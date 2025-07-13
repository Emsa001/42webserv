/*
  Given a config_array of servers will set up sockets and accept requests
  redirecting them to the appropriate servers.
*/
#ifndef SOCKETHANDLER_HPP
#define SOCKETHANDLER_HPP

#include "Webserv.hpp"

#ifndef TCP_KEEPIDLE
    #define TCP_KEEPIDLE 4
#endif

class Server;
class HttpRequest;
struct ClientRequestState;

class SocketHandler {
private:
    std::vector<Server> _servers;
    int _num_sockets;

    // connections
    std::map<int, int> _fds_to_ports;
    // pollfds vector to be passed on to poll()
    std::vector<pollfd> _pollfds;

    /* the _pollfds vector is an array of `struct pollfd` able to be passed to poll()
    connection file descriptors are appended to the end as needed

    | socket_fds | connection_fds |
                 ^
                _num_sockets
    
    */
    // std::map<int, time_t> client_timestamps;

    // connections are identified by their file descriptors
    //  we store any state belonging to a connection (such as buffer)
    //  in the ClientRequestState struct
    std::map<int, ClientRequestState> _conns;

    // TODO: Move this to SocketHandler
    // void listener(int server_sock);
    // void setNonBlocking(int sock);
    // void acceptNewConnections(int server_sock);
    // void handleClientRead(size_t index);
    // void checkIdleClients();
    // void removeClient(size_t index);

    // TODO: keep-alive
    // this->keep_alive = Config::getSafe(config, "keep_alive", 30).getInt();;
    
    // TODO: Move this to SocketHandler
    ClientRequestState *readChunk(int fd, int index, char *buffer);
    int readBytes(int fd, int index, char *buffer);

    // Determine server responsible for request (based on port and "Host" header)
    Server &determineServer(HttpRequest &req, int port);

    bool portTaken(int port);

    // Close connections that have been idle for keepAlive ms.
    // void unaliveConnections() {
    //     time_t now = time(NULL);

    //     for (size_t i = 1; i < _conns.size(); ++i) {
    //         int fd = fds[i].fd;

    //         if (now - client_timestamps[fd] > this->keep_alive) {
    //             // std::cout << "Idle timeout, closing fd: " << fd << std::endl;
    //             Logger::clientIdle(fd);
    //             this->removeClient(i);
    //             --i;
    //         }
    //     }
    // }

public:
    SocketHandler(const config_array& servers);
    ~SocketHandler();

    bool InitSockets();
    int run();

    void processConnection(int i);
    void processData(int i);
};

#endif // SOCKETHANDLER_HPP