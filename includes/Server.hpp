#ifndef SERVER_HPP
#define SERVER_HPP

#include "Webserv.hpp"

class HttpRequest;
class HttpResponse;
struct ClientRequestState;

class Server
{
private:
    config_map config;
    // Helper methods
    bool isValidMethod(HttpRequest *request, const config_map &location);
    bool isRedirect(HttpResponse &response, const config_map &location);

    const FileData createFileData(const config_map *location, std::string path) const;

public:
    Server(const config_map &config) : config(config) {}
    ~Server() {}

    const config_map *findLocation(const std::string &path);
    const config_map *findByExtension(const std::string &path);
    // TODO: Move this to SocketHandler
    void closeConnection(int *client_sock);

    // Startpoint for the server
    HttpResponse handleResponse(HttpRequest *request);

    // Getters

    std::string const getServerName() const { return this->config.at("server_name"); }
    config_map const &getConfig() const { return this->config; }
};

#endif
