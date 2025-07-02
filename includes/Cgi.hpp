#ifndef CGI_HPP
#define CGI_HPP

#include "Webserv.hpp"
#include "HttpResponse.hpp"
class HttpResponse;

class Cgi
{
    private:
        enum Type { 
            PYTHON,
            PHP,
            SHELL,
            PERL,
            UNKNOWN 
        };

        char **convert_env(const std::map<std::string, std::string>& env_map);
        std::map<std::string, std::string> get_env(const std::string& scriptPath, const HttpRequest *request);
        Type detect_type(const std::string &scriptPath);
        std::string get_interpreter(Type type);

        std::string get_query(const std::string &uri);
        std::string toEnvFormat(const std::string &header);

    public:
        void execute(const std::string &scriptPath, HttpResponse *response, const HttpRequest *request);
};

std::string get_body(const std::string &output);
void set_headers(HttpResponse *response, const std::string &output);

#endif
