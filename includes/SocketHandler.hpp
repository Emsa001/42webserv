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
public:
    SocketHandler(config_array& servers);
    SocketHandler();
    ~SocketHandler();

    int run();
};

#endif // SOCKETHANDLER_HPP