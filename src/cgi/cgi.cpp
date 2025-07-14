#include "Cgi.hpp"

static char *str_char(const std::string &str)
{
    return const_cast<char *>(str.c_str());
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
        if(code != 200)
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

    if(child)
        dup2(output_pipe[1], STDOUT_FILENO);
    
    close(output_pipe[1]);
    if(!child)
        output = read_output(output_pipe[0]);
    close(output_pipe[0]);

    if(child)
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

    // Close write end of input pipe and read end of output pipe in parent
    close(input_pipe[1]);
    close(output_pipe[1]);

    int status;
    time_t start_time = time(NULL);
    bool timeout_occurred = false;
    
    while (true)
    {
        pid_t result = waitpid(pid, &status, WNOHANG);
        
        if (result > 0)
            break; // Child process has finished
        else if (result == -1)
            throw HttpRequestException(500); // waitpid error
        
        // Check for timeout
        if (time(NULL) - start_time >= CGI_TIMEOUT)
        {
            timeout_occurred = true;
            kill(pid, SIGKILL); // Force kill the child process
            waitpid(pid, &status, 0); // Wait for the killed process to be reaped
            break;
        }
        
        usleep(10000); // Sleep for 10ms to avoid busy waiting
    }

    if (timeout_occurred)
    {
        close(output_pipe[0]);
        close(input_pipe[0]);
        throw HttpRequestException(504); // Gateway Timeout
    }

    if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
    {
        close(output_pipe[0]);
        close(input_pipe[0]);
        throw HttpRequestException(500);
    }

    // Now read the output after we know the process has finished
    std::string output = read_output(output_pipe[0]);
    close(output_pipe[0]);
    close(input_pipe[0]);

    set_headers(response, output);
    cgi_response(get_body(output), response, 200);
}
