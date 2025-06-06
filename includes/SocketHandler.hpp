/*
  Given a config_array of servers will set up sockets and accept requests
  redirecting them to the appropriate servers.
*/
#ifndef SOCKETHANDLER_HPP
#define SOCKETHANDLER_HPP

#include "Webserv.hpp"

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
public:
    SocketHandler(config_array& servers);
    SocketHandler();
    ~SocketHandler();

    bool InitSockets();
    int run();
};

#endif // SOCKETHANDLER_HPP