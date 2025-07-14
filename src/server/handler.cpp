#include "SocketHandler.hpp"
#include "Webserv.hpp"

SocketHandler::SocketHandler(const config_array &servers) : _num_sockets(0)
{
    Logger::debug("Initialializing SocketHandler");

    // init servers for request handling
    for (config_array::const_iterator it = servers.begin(); it != servers.end(); ++it)
    {
        Server server(*it);
        _servers.push_back(server);
    }
}

SocketHandler::~SocketHandler()
{
    Logger::debug("Deconstructing SocketHandler");
}

Server &SocketHandler::determineServer(HttpRequest &req, int port)
{
    std::string hostHeader = req.getHeader("Host");

    // Extract hostname from Host header
    std::string hostname = hostHeader;
    size_t colonPos = hostname.find(':');
    if (colonPos != std::string::npos)
        hostname = hostname.substr(0, colonPos);

    // Find server by port and server_name
    for (size_t i = 0; i < _servers.size(); ++i)
    {
        std::string const listen = Config::getSafe(_servers[i].getConfig(), "listen", "").getString();
        std::string const serverName = Config::getSafe(_servers[i].getConfig(), "server_name", "").getString();

        if (stringToInt(listen) == port && (serverName == hostname || serverName == hostHeader))
        {
            return _servers[i];
        }
    }

    // TODO ignore request
    return _servers[0];
}

// determines whether any open fd already points to given `port`
bool SocketHandler::portTaken(int port)
{
    for (std::map<int, int>::const_iterator it = _fds_to_ports.begin(); it != _fds_to_ports.end(); ++it)
    {
        if (it->second == port)
            return true;
    }
    return false;
}

bool SocketHandler::InitSockets()
{
    struct addrinfo hints, *res;
    int s;

    memset(&hints, 0, sizeof hints); // make sure the struct is empty
    hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
    hints.ai_flags = AI_PASSIVE;     //

    // Group servers by port to handle virtual hosts
    std::map<int, std::vector<size_t> > portToServers;
    for (size_t i = 0; i < _servers.size(); ++i)
    {
        std::string const port = Config::getSafe(_servers[i].getConfig(), "listen");
        int port_int = stringToInt(port);
        portToServers[port_int].push_back(i);
    }

    // Create sockets for each unique port
    for (std::map<int, std::vector<size_t> >::iterator portIt = portToServers.begin(); portIt != portToServers.end();
         ++portIt)
    {
        int port_int = portIt->first;
        std::string port = intToString(port_int);
        std::string const host = "0.0.0.0";

        int r;
        if ((r = getaddrinfo(host.c_str(), port.c_str(), &hints, &res)) < 0)
            perror("getaddrinfo");

        struct pollfd newfd;
        newfd.events = POLLIN;
        newfd.revents = 0;
        newfd.fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        _fds_to_ports[newfd.fd] = port_int;
        int sock = newfd.fd;

        // Enable socket reuse
        int optval = 1;
        setsockopt(newfd.fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

        // Disable TCP Keepalive for listening socket (will be set per connection)
        optval = 0;
        setsockopt(newfd.fd, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval));

        int keep_idle = 60;     // Start checking after 10 second of inactivity
        int keep_interval = 30; // Send keep-alive probes every 5 second
        int keep_count = 3;     // Disconnect after 3 failed probes
        setsockopt(sock, IPPROTO_TCP, TCP_KEEPIDLE, &keep_idle, sizeof(keep_idle));
        setsockopt(sock, IPPROTO_TCP, TCP_KEEPINTVL, &keep_interval, sizeof(keep_interval));
        setsockopt(sock, IPPROTO_TCP, TCP_KEEPCNT, &keep_count, sizeof(keep_count));

        Logger::info(" socket() = " + intToString(sock));

        s = bind(newfd.fd, res->ai_addr, res->ai_addrlen);
        Logger::info(" bind(port=" + port + ") = " + intToString(s));

        s = listen(newfd.fd, 10);
        Logger::info(" listen() =  " + intToString(s));

        _pollfds.push_back(newfd);
        _num_sockets++;

        freeaddrinfo(res);
    }

    return true;
}

void SocketHandler::processConnection(int i)
{
    Logger::debug("New connection");
    // accept connection and add it to the fds to watch
    struct pollfd *it = &_pollfds[i];
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);
    struct pollfd newconn;
    newconn.events = POLLIN;
    newconn.revents = 0;
    newconn.fd = accept(it->fd, (sockaddr *)&addr, &addrlen);
    if (newconn.fd < 0)
    {
        perror("error5: ");
        Logger::error("accept() error");
        // continue;//TODO
    }
    // TODO: get port (getsockname) and store in newconn
    // std::cout << newconn.fd << std::endl;

    if (newconn.fd < 0)
    {
        Logger::error("Connection error - invalid fd");
    }

    std::map<int, ClientRequestState>::iterator exstReq = _conns.find(it->fd);
    if (exstReq != _conns.end())
    {
        // TODO: error, request exists?!
    }
    ClientRequestState newReq;
    newReq.port = _fds_to_ports[it->fd];
    // std::cout << newReq.port <<
    newReq.contentLength = 0;
    newReq.keepalive = false;
    newReq.expectedSize = -1;
    newReq.headersComplete = false;
    newReq.request = HttpRequest("");
    newReq.buffer.clear();
    newReq.server = NULL;

    // create new connection, consisting of request and pollfd
    _conns[newconn.fd] = newReq;
    // append to pollfds array
    _pollfds.push_back(newconn);
}

void SocketHandler::processData(int i)
{
    _pollfds[i].revents = 0;
    struct pollfd *it = &_pollfds[i];

    // Logger::debug("data from conn");
    char buffer[READ_BUFFER_SIZE];
    int res = recv(it->fd, buffer, sizeof(buffer), 0);
    // TODO: error check `res`
    if (res < 0)
    {
        Logger::error("recv() error");
        // std::cout << it->fd << std::endl;
        it->revents = 0;
        // continue;//TODO
    }

    bool keep_reading =
        _conns[it->fd].request.feed(buffer, res,
                                    (_conns[it->fd].server ? _conns[it->fd].server->getConfig()
                                                           : _servers[0].getConfig())); // TODO: get correct config
    Logger::debug("extended buffer");
    if (_conns[it->fd].request.getHeadersComplete())
        Logger::info(_conns[it->fd].request.getMethod() + " " + _conns[it->fd].request.getURI());
    // the config is just for max, is ok for now
    if (!_conns[it->fd].request.getHeadersComplete())
        // continue;//TODO
        return;

    Logger::debug(_conns[it->fd].request.getRawRequestData());

    // figure out where to direct request
    _conns[it->fd].server = &determineServer(_conns[it->fd].request, _conns[it->fd].port);

    // set socket timeout if keep-alive is set
    std::string connection_header = _conns[it->fd].request.getHeader("connection");
    if (!connection_header.empty())
    {
        int keep_alive = Config::getSafe(_conns[it->fd].server->getConfig(), "keep_alive", 0);

        Logger::debug("keepalive is " + intToString(keep_alive));
        _conns[it->fd].keepalive = keep_alive;

        struct timeval sock_timeout;
        sock_timeout.tv_usec = 0;
        sock_timeout.tv_sec = keep_alive;
        int optval = 0;
        setsockopt(it->fd, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval));
        setsockopt(it->fd, SOL_SOCKET, SO_RCVTIMEO, &sock_timeout, sizeof(sock_timeout));
        setsockopt(it->fd, SOL_SOCKET, SO_SNDTIMEO, &sock_timeout, sizeof(sock_timeout));
    }

    // if (!_conns[it->fd].keepalive)
    //     shutdown(it->fd, 0); // no reads anymore

    HttpResponse response = _conns[it->fd].server->handleResponse(&_conns[it->fd].request);
    if (response.isInvalid()) // TODO: wait until request.isBodyComplete()
        return;
    std::string responseStr = response.getResponse();

    if (send(it->fd, responseStr.c_str(), responseStr.size(), 0) < 0)
    {
        Logger::error();
        perror("error3: ");
    }
    Logger::debug("sent response");

    _conns[it->fd].buffer.clear();
    _conns[it->fd].contentLength = -1;
    _conns[it->fd].headersComplete = false;
    _conns[it->fd].request = HttpRequest();
    if (!_conns[it->fd].keepalive)
    {
        close(it->fd);
        _conns.erase(it->fd);
        _pollfds.erase(_pollfds.begin() + i);
        Logger::debug("closed connection");
    }
    if (res <= 0)
    {
        Logger::info("removing");
        perror("error2: ");
    }
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
            Logger::error();
            perror("err1: ");
            return -2;
        }
        // Looping backwards to prevent iterator invalidation
        for (int i = _pollfds.size() - 1; i >= 0; --i)
        {
            struct pollfd *it = &_pollfds[i];
            if (it->revents & POLLIN)
            {
                if (i < _num_sockets)
                    // event was on socket
                    processConnection(i);
                else
                    // event was on open connection
                    processData(i);
            }
            it->revents = 0;
        }
    }
}
