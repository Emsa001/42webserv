#include "Cgi.hpp"

static char *str_char(const std::string &str)
{
    return const_cast<char *>(str.c_str());
}

// Get current time in seconds
static time_t get_current_time()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec;
}

std::string read_output(int pipe_fd)
{
    char buffer[1024];
    std::stringstream response;
    ssize_t bytesRead;
    
    while ((bytesRead = read(pipe_fd, buffer, sizeof(buffer) - 1)) > 0)
    {
        buffer[bytesRead] = '\0';
        response << buffer;
    }
    return response.str();
}

void cgi_response(const std::string &message, HttpResponse *response, short code)
{
    if (message.empty() || message.size() <= 1)
        throw HttpRequestException(500);

    response->setStatusCode(code);

    StringMultiMap headers = response->getHeaders();

    if (headers.find("Content-Type") == headers.end())
    {
        if (code != 200)
            response->setHeader("Content-Type", "text/plain");
        else
            response->setHeader("Content-Type", "text/html");
    }
    if (headers.find("Content-Length") == headers.end())
        response->setHeader("Content-Length", intToString(message.size()));

    response->setBody(message);
    response->build();
}

std::string close_pipes(int output_pipe[2], int input_pipe[2], bool child)
{
    std::string output;

    if (child)
        dup2(output_pipe[1], STDOUT_FILENO);

    close(output_pipe[1]);
    if (!child)
        output = read_output(output_pipe[0]);
    close(output_pipe[0]);

    if (child)
        dup2(input_pipe[0], STDIN_FILENO);
    close(input_pipe[1]);
    close(input_pipe[0]);

    return output;
}

void Cgi::execute(const std::string &scriptPath, HttpResponse *response, const HttpRequest *request)
{
    Type scriptType = detect_type(scriptPath);

    if (scriptType == UNKNOWN)
        throw HttpRequestException(415);

    int output_pipe[2];
    int input_pipe[2];

    if (pipe(output_pipe) == -1 || pipe(input_pipe) == -1)
        throw HttpRequestException(500);

    pid_t pid = fork();
    if (pid < 0)
    {
        close_pipes(output_pipe, input_pipe, false);
        throw HttpRequestException(500);
    }

    if (pid == 0)
    {
        close_pipes(output_pipe, input_pipe, true);
        char *args[2];
        args[0] = str_char(scriptPath);
        args[1] = NULL;
        StringMap envMap = this->get_env(scriptPath, request);
        char **env = convert_env(envMap);

        if (execve(scriptPath.c_str(), args, env) == -1)
        {
            perror("execve failed");
            close_pipes(output_pipe, input_pipe, false);
            throw HttpRequestException(500);
        }
    }

    if (request->getMethod() == "POST" || request->getMethod() == "DELETE")
    {
        const std::string &body = request->getBody();
        if (!body.empty())
            write(input_pipe[1], body.c_str(), body.size());
    }
    
    close(input_pipe[1]); // Close write end of input pipe

    // Record start time for timeout
    time_t start_time = get_current_time();
    bool timeout_occurred = false;
    int status;
    std::string output;

    // Wait for child process with timeout
    while (true)
    {
        // Check if server is being stopped
        if (stop)
        {
            // Server shutdown requested, terminate CGI immediately
            kill(pid, 9);
            waitpid(pid, &status, 0);
            close(output_pipe[0]);
            close(input_pipe[0]);
            throw HttpRequestException(503); // Service Unavailable
        }
        
        pid_t result = waitpid(pid, &status, WNOHANG);
        
        if (result == pid)
        {
            // Child process has finished
            break;
        }
        else if (result == 0)
        {
            // Child is still running, check timeout
            time_t current_time = get_current_time();
            if (current_time - start_time >= CGI_TIMEOUT)
            {
                timeout_occurred = true;
                break;
            }
            // Brief sleep to avoid busy waiting
            usleep(10000); // 10ms
        }
        else
        {
            // Error in waitpid
            close(output_pipe[0]);
            close(input_pipe[0]);
            throw HttpRequestException(500);
        }
    }

    if (timeout_occurred)
    {
        // Terminate the child process if timeout occurred
        // First, close our ends of the pipes to signal the child
        close(output_pipe[0]);
        close(input_pipe[0]);
        
        // Wait briefly to see if the child terminates on its own
        // when it detects the closed pipes
        time_t term_start = get_current_time();
        while (get_current_time() - term_start < 1)
        {
            pid_t result = waitpid(pid, &status, WNOHANG);
            if (result == pid)
            {
                // Process terminated on its own
                throw HttpRequestException(504); // Gateway Timeout
            }
            usleep(50000); // 50ms
        }
        
        // If process is still running, we have no choice but to force kill it
        // This is unavoidable for truly infinite loops
        kill(pid, 9); // Use raw signal number instead of SIGKILL
        waitpid(pid, &status, 0);
        throw HttpRequestException(504); // Gateway Timeout
    }

    // Read output from child process
    output = read_output(output_pipe[0]);
    
    close(output_pipe[0]);
    close(input_pipe[0]);

    if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
        throw HttpRequestException(500);

    set_headers(response, output);
    cgi_response(get_body(output), response, 200);
}
