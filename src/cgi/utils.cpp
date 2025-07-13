#include "Cgi.hpp"

bool ends_with(const std::string &str, const std::string &suffix)
{
    if (str.size() >= suffix.size())
        if (str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0)
            return true;
    return false;
}

Cgi::Type Cgi::detect_type(const std::string &scriptPath)
{
    if (ends_with(scriptPath, ".py"))
        return PYTHON;
    if (ends_with(scriptPath, ".php"))
        return PHP;
    if (ends_with(scriptPath, ".sh"))
        return SHELL;
    if (ends_with(scriptPath, ".pl"))
        return PERL;
    return UNKNOWN;
}

std::string get_body(const std::string &output)
{
    std::string body;
    std::istringstream stream(output);
    std::string line;
    bool is_body = false;

    while (std::getline(stream, line))
    {
        if (!line.empty() && *line.rbegin() == '\r')
            line.erase(line.size() - 1);

        if (!is_body && line.empty())
        {
            is_body = true;
            continue;
        }

        if (is_body)
        {
            body += line;
            body += "\n";
        }
    }

    return body;
}

// TODO: Check what is that, can't we just use getHeaders and setHeader from
// HttpMessage?

StringMultiMap get_headers(const std::string &output)
{
    StringMultiMap headers;
    std::istringstream stream(output);
    std::string line;

    while (std::getline(stream, line))
    {
        if (!line.empty() && *line.rbegin() == '\r')
            line.erase(line.size() - 1);

        if (line.empty())
            break;

        size_t pos = line.find(':');
        if (pos != std::string::npos)
        {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);

            if (!value.empty() && value[0] == ' ')
                value.erase(0, 1);

            headers.insert(std::make_pair(key, value));
        }
    }

    return headers;
}

void set_headers(HttpResponse *response, const std::string &output)
{
    StringMultiMap headers = get_headers(output);
    if (headers.empty())
        throw HttpRequestException(500);

    for (StringMap::iterator it = headers.begin(); it != headers.end(); ++it)
    {
        response->setHeader(it->first, it->second);
    }
}
