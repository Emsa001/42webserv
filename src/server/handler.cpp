#include "Webserv.hpp"
#include "SocketHandler.hpp"

SocketHandler::SocketHandler(config_array &servers) : _servers(servers)
{
    std::cout << "SocketHandler()" << std::endl;
}

SocketHandler::SocketHandler()
{
    Config &config = Config::instance();
    config.parse("conf/default.yml");
    _servers = config.getServers();
}

SocketHandler::~SocketHandler()
{
    std::cout << "~SocketHandler" << std::endl;
}

bool SocketHandler::InitSockets()
{
    struct addrinfo hints, *res;
    int r;
    if (r = getaddrinfo("127.0.0.1", "8080", NULL, &res) < 0)
        perror("getaddrinfo");
    struct pollfd newfd = (struct pollfd){ .events = POLLIN, .revents = 0 };
    // std::cout << res->ai_family << std::endl;
    newfd.events = POLLIN;
    newfd.revents = 0;
    newfd.fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    int sock = newfd.fd;
    int flags = fcntl(sock, F_GETFL, 0);
    fcntl(sock, F_SETFL, flags | O_NONBLOCK);
    
    // Enable TCP Keepalive
    int optval = 1;
    setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval));

    int keep_idle = 10;     // Start checking after 10 second of inactivity
    int keep_interval = 5;  // Send keep-alive probes every 5 second
    int keep_count = 3;     // Disconnect after 3 failed probes
    
    setsockopt(sock, IPPROTO_TCP, TCP_KEEPIDLE, &keep_idle, sizeof(keep_idle));
    setsockopt(sock, IPPROTO_TCP, TCP_KEEPINTVL, &keep_interval, sizeof(keep_interval));
    setsockopt(sock, IPPROTO_TCP, TCP_KEEPCNT, &keep_count, sizeof(keep_count));

    std::cout << "socket fd is " << newfd.fd << std::endl;
    std::cout << "bind: " << bind(newfd.fd, res->ai_addr, res->ai_addrlen) << std::endl;
    std::cout << "listen: " << listen(newfd.fd, 10) << std::endl;
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
        // std::cout << _sockets[0].revents << std::endl;
        if (poll((struct pollfd *)_pollfds.data(), _pollfds.size(), -1) < 0)
        {
            perror("err: ");
            return -2;
        }
        std::cout << "poll is over " << std::endl;
        for (std::vector<struct pollfd>::iterator it = _pollfds.begin(); it != _pollfds.end(); ++it)
        {
            int i = std::distance(_pollfds.begin(), it);

            struct sockaddr_in addr;
            socklen_t addrlen;
            if (it->revents & POLLIN) {
                // the event was on a socket
                if (i < _num_sockets) {
                    std::cout << "New connection" << std::endl;
                    // accept connection and add it to the fds to watch
                    struct pollfd newconn;
                    newconn.fd = accept(it->fd, (sockaddr *) &addr, &addrlen);
                    if (newconn.fd < 0)
                        perror("error: ");
                    newconn.events = POLLIN;
                    newconn.revents = 0;
                    std::cout << "new fd is " << newconn.fd << std::endl;
                    // append to pollfds array
                    _pollfds.push_back(newconn);
                    // reset receivecd events
                    // event on active connection
                // the event was on an open connection
                } else {
                    std::cout << "read" << std::endl;
                    char buffer[128];
                    int res = read(it->fd, buffer, 128);
                    if (res < 0) {
                        perror("error: ");
                    }
                }
            }
            it->revents = 0;
        }
    }
}