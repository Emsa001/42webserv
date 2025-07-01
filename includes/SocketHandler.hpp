/*
  Given a config_array of servers will set up sockets and accept requests
  redirecting them to the appropriate servers.
*/
#ifndef SOCKETHANDLER_HPP
#define SOCKETHANDLER_HPP

#include "Webserv.hpp"

struct ClientRequestState;
class SocketHandler {
private:
    config_array _servers;
    std::vector<pollfd> _pollfds;
    int _num_sockets;

    /* the _pollfds vector is an array of `struct pollfd` able to be passed to poll()
    connection file descriptors are appended to the end as needed

    | socket_fds | connection_fds |
                 ^
                _num_sockets
    
    */
    // TODO: Move this to SocketHandler
    std::vector<pollfd> fds;
    // std::map<int, time_t> client_timestamps;
    std::map<int, ClientRequestState> _requests;

    // TODO: Move this to SocketHandler
    // void listener(int server_sock);
    // void setNonBlocking(int sock);
    // void acceptNewConnections(int server_sock);
    // void handleClientRead(size_t index);
    // void checkIdleClients();
    // void removeClient(size_t index);

    // TODO: Move this to SocketHandler
    ClientRequestState *readChunk(int fd, int index, char *buffer);
    int readBytes(int fd, int index, char *buffer);

public:
    SocketHandler(const config_array& servers);
    SocketHandler();
    ~SocketHandler();

    bool InitSockets();
    int run();
};

#endif // SOCKETHANDLER_HPP