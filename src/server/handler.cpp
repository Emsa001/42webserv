#include "Webserv.hpp"
#include "SocketHandler.hpp"

#include <string>

SocketHandler::SocketHandler(const config_array& servers)
{
    std::cout << "SocketHandler()" << std::endl;

    // init servers for request handling
    for (config_array::const_iterator it = servers.begin(); it != servers.end(); ++it) {
        Server server(*it);
        _servers.push_back(server);
    }
}

// SocketHandler::SocketHandler()
// {
//     Config &config = Config::instance();
//     config.parse("conf/default.yml");
//     _serversConfig = config.getServers();
// }

SocketHandler::~SocketHandler()
{
    std::cout << "~SocketHandler" << std::endl;
}

bool SocketHandler::InitSockets()
{
    struct addrinfo hints, *res;
    int s;

    memset(&hints, 0, sizeof hints); // make sure the struct is empty
    hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
    hints.ai_flags = AI_PASSIVE;     //
    
    std::string const host = "0.0.0.0";//Config::getSafe(_servers[0].getConfig(), "listen");
    std::string const port = Config::getSafe(_servers[0].getConfig(), "listen");
    int r;
    if (r = getaddrinfo(host.c_str(), port.c_str(), &hints, &res) < 0)
        perror("getaddrinfo");
    // struct pollfd newfd = (struct pollfd){ .events = POLLIN, .revents = 0 };
    struct pollfd newfd;
    // std::cout << res->ai_family << std::endl;
    newfd.events = POLLIN;
    newfd.revents = 0;
    newfd.fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    int sock = newfd.fd;

    // // Enable TCP Keepalive
    int optval = 1;
    setsockopt(newfd.fd, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval));
    setsockopt(newfd.fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    
    int keep_idle = 10;     // Start checking after 10 second of inactivity
    int keep_interval = 5;  // Send keep-alive probes every 5 second
    int keep_count = 3;     // Disconnect after 3 failed probes
    setsockopt(sock, IPPROTO_TCP, TCP_KEEPIDLE, &keep_idle, sizeof(keep_idle));
    setsockopt(sock, IPPROTO_TCP, TCP_KEEPINTVL, &keep_interval, sizeof(keep_interval));
    setsockopt(sock, IPPROTO_TCP, TCP_KEEPCNT, &keep_count, sizeof(keep_count));

    Logger::info(" socket() = " + intToString(sock));

    s = bind(newfd.fd, res->ai_addr, res->ai_addrlen);
    Logger::info(" bind() = " + intToString(s));

    s = listen(newfd.fd, 10);
    Logger::info(" listen() =  " + intToString(s));

    _pollfds.push_back(newfd);
    _num_sockets = 1;
    return true;
}

int SocketHandler::run()
{
    if (!InitSockets())
        return -1;

    // Server main control loop
    while (true)
    {
        if (poll((struct pollfd *)_pollfds.data(), _pollfds.size(), -1) < 0)
        {
            perror("err: ");
            return -2;
        }
        // Looping backwards to prevent iterator invalidation
        // TODO: make this work with (reverse) iterators
        for (int i=_pollfds.size(); i>=0; --i) {
            struct pollfd *it = &_pollfds[i];
            struct sockaddr_in addr;
            socklen_t addrlen;
            
            if (it->revents & POLLIN) {
                // event was on socket
                if (i < _num_sockets) {
                    Logger::debug("New connection");
                    // accept connection and add it to the fds to watch
                    struct pollfd newconn;
                    newconn.events = POLLIN;
                    newconn.revents = 0;
                    newconn.fd = accept(it->fd, (sockaddr *) &addr, &addrlen);
                    if (newconn.fd < 0) {
                        Logger::error("Connection error - invalid fd");
                    }

                    std::map<int, ClientRequestState>::iterator exstReq = _conns.find(it->fd);
                    if (exstReq != _conns.end()) {
                        // TODO: error, request exists?!
                    }
                    ClientRequestState newReq;
                    newReq.contentLength = 0;
                    newReq.expectedSize = -1;
                    newReq.headersParsed = false;
                    newReq.buffer.clear();

                    // create new connection, consisting of request and pollfd
                    _conns[newconn.fd] = newReq;
                    // append to pollfds array
                    _pollfds.push_back(newconn);
                // event was on open connection
                } else {
                    // Logger::debug("data from conn");
                    char buffer[4096]; // TODO: BUFSIZE
                    int res = recv(it->fd, buffer, sizeof(buffer), 0);
                    // TODO: error check `res`
                    _conns[it->fd].buffer.append(buffer, res);
                    shutdown(it->fd, 0); // no reads anymore

                    HttpRequest request(_conns[it->fd].buffer, _servers[0].getConfig());
                    Logger::info(request.getMethod() + " " + request.getURI());
                    HttpResponse response = _servers.at(0).handleResponse(&request);
                    std::string responseStr = response.getResponse();
                    if (send(it->fd, responseStr.c_str(), responseStr.size(), 0) < 0)
                        perror("error3: ");
                    close(it->fd);
                    _pollfds.erase(_pollfds.begin() + i);
                    if (res <= 0) {
                        Logger::info("removing");
                        perror("error2: ");
                    }

                    // HttpRequest r();
                }
            }
            it->revents = 0;
        }
    }
}