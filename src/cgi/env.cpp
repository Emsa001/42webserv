#include "Cgi.hpp"

std::string Cgi::get_query(const std::string &uri)
{
    size_t pos = uri.find('?');
    if (pos != std::string::npos)
    {
        return uri.substr(pos + 1);
    }
    return "";
}

std::string Cgi::toEnvFormat(const std::string &header)
{
    std::string result;
    for (size_t i = 0; i < header.length(); ++i)
    {
        char c = header[i];
        if (c == '-')
            result += '_';
        else
            result += std::toupper(c);
    }
    return result;
}

StringMap Cgi::get_env(const std::string &scriptPath, const HttpRequest *request)
{
    std::string query = this->get_query(request->getURI());

    StringMap env;

    // Standard CGI variables
    env["GATEWAY_INTERFACE"] = "CGI/1.1";
    env["SCRIPT_FILENAME"] = scriptPath;
    env["REQUEST_METHOD"] = request->getMethod();
    env["QUERY_STRING"] = query;
    env["SERVER_PROTOCOL"] = "HTTP/1.1";
    env["SERVER_SOFTWARE"] = PROJECT_NAME;

    // Server & request info
    env["SERVER_NAME"] = request->getHeader("Host");
    env["SERVER_PORT"] = request->getPort();
    env["REQUEST_URI"] = request->getURI();

    // Headers (prefixed with HTTP_)
    const std::string headers_arr[] = {"Host",       "User-Agent", "Accept", "Accept-Language", "Accept-Encoding",
                                       "Connection", "Referer",    "Cookie"};
    const std::vector<std::string> headers(headers_arr, headers_arr + sizeof(headers_arr) / sizeof(headers_arr[0]));

    for (size_t i = 0; i < headers.size(); ++i)
    {
        std::string header = headers[i];
        std::string value = request->getHeader(header);
        if (!value.empty())
        {
            std::string key = "HTTP_" + this->toEnvFormat(header);
            env[key] = value;
        }
    }

    // POST/DELETE handling
    if (request->getMethod() == "POST" || request->getMethod() == "DELETE")
    {
        env["CONTENT_LENGTH"] = intToString(request->getBody().size());
        std::string contentType = request->getHeader("Content-Type");
        if (!contentType.empty())
            env["CONTENT_TYPE"] = contentType;
    }

    return env;
}

char **Cgi::convert_env(const StringMap &env_map)
{
    char **env = new char *[env_map.size() + 1];
    size_t i = 0;

    for (StringMap::const_iterator it = env_map.begin(); it != env_map.end(); ++it)
    {
        std::string entry = it->first + "=" + it->second;
        env[i] = new char[entry.size() + 1];
        std::strcpy(env[i], entry.c_str());
        ++i;
    }

    env[i] = NULL;
    return env;
}
