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

    // TODO: Move this to SocketHandler
    std::vector<pollfd> fds;
    std::map<int, time_t> client_timestamps;
    std::map<int, ClientRequestState> requestStates;

    // TODO: Move this to SocketHandler
    void listener(int server_sock);
    void setNonBlocking(int sock);
    void acceptNewConnections(int server_sock);
    void handleClientRead(size_t index);
    void checkIdleClients();
    void removeClient(size_t index);

    // TODO: Move this to SocketHandler
    ClientRequestState *readChunk(int fd, int index, char *buffer);
    int readBytes(int fd, int index, char *buffer);

    // Helper methods
    bool isValidMethod(HttpRequest *request, const config_map &location);
    bool isRedirect(HttpResponse &response, const config_map &location);

    const FileData createFileData(const config_map *location, HttpRequest *request) const;

public:
    Server(const config_map &config) : config(config)
    {
        // TODO: Handle this in SocketHandler
        // this->keep_alive = Config::getSafe(config, "keep_alive", 30).getInt();;
    }
    ~Server() {}

    const config_map *findLocation(const std::string &path);
    // TODO: Move this to SocketHandler
    void closeConnection(int *client_sock);

    // Startpoint for the server
    HttpResponse handleResponse(HttpRequest *request);

    // Getters

    std::string const getServerName() const { return this->config.at("server_name"); }
    config_map const &getConfig() const { return this->config; }
};

#endif
