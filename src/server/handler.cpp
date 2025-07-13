#include "Webserv.hpp"
#include "SocketHandler.hpp"

SocketHandler::SocketHandler(const config_array& servers): _num_sockets(0)
{
    Logger::debug("Initialializing SocketHandler");

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
    Logger::debug("Deconstructing SocketHandler");
}

Server & SocketHandler::determineServer(HttpRequest &req, int port) {
    std::string hostHeader = req.getHeader("Host");
    
    // Extract hostname from Host header
    std::string hostname = hostHeader;
    size_t colonPos = hostname.find(':');
    if (colonPos != std::string::npos)
        hostname = hostname.substr(0, colonPos);


    // Find server by port and server_name
    for (size_t i = 0; i < _servers.size(); ++i) {
        std::string const listen = Config::getSafe(_servers[i].getConfig(), "listen", "").getString();
        std::string const serverName = Config::getSafe(_servers[i].getConfig(), "server_name", "").getString();

        if (stringToInt(listen) == port && (serverName == hostname || serverName == hostHeader)) {
            return _servers[i];
        }
    }
    
    // TODO ignore request
    return _servers[0];
}

bool SocketHandler::portTaken(int port) {
    for (std::map<int, int>::const_iterator it = _fds_to_ports.begin(); it != _fds_to_ports.end(); ++it) {
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
    for (size_t i = 0; i < _servers.size(); ++i) {
        std::string const port = Config::getSafe(_servers[i].getConfig(), "listen");
        int port_int = stringToInt(port);
        portToServers[port_int].push_back(i);
    }
    
    // Create sockets for each unique port
    for (std::map<int, std::vector<size_t> >::iterator portIt = portToServers.begin(); 
         portIt != portToServers.end(); ++portIt) {
        
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
        
        int keep_idle = 10;     // Start checking after 10 second of inactivity
        int keep_interval = 5;  // Send keep-alive probes every 5 second
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
        // TODO: make this work with (reverse) iterators
        for (int i=_pollfds.size()-1; i>=0; --i) {
            struct pollfd *it = &_pollfds[i];
            struct sockaddr_in addr;
            socklen_t addrlen = sizeof(addr);
            
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
                        perror("error5: ");
                        Logger::error("accept() error");
                        continue;
                    }
                    // TODO: get port (getsockname) and store in newconn
                    // std::cout << newconn.fd << std::endl;

                    if (newconn.fd < 0) {
                        Logger::error("Connection error - invalid fd");
                    }

                    std::map<int, ClientRequestState>::iterator exstReq = _conns.find(it->fd);
                    if (exstReq != _conns.end()) {
                        // TODO: error, request exists?!
                    }
                    ClientRequestState newReq;
                    newReq.port = _fds_to_ports[it->fd];
                    // std::cout << newReq.port <<
                    newReq.contentLength = 0;
                    newReq.keepalive = false;
                    newReq.expectedSize = -1;
                    newReq.headersParsed = false;
                    newReq.request = HttpRequest("");
                    newReq.buffer.clear();

                    // create new connection, consisting of request and pollfd
                    _conns[newconn.fd] = newReq;
                    // append to pollfds array
                    _pollfds.push_back(newconn);
                // event was on open connection
                } else {
                    it->revents = 0;
                    
                    // Logger::debug("data from conn");
                    char buffer[READ_BUFFER_SIZE];
                    // TODO: ! This assumes request can be read non-blocking as once!
                    int res = recv(it->fd, buffer, sizeof(buffer), 0);
                    // TODO: error check `res`
                    if (res < 0) {
                        Logger::error("recv() error");
                        std::cout << it->fd << std::endl;
                        it->revents = 0;
                        continue;
                    }
                    bool complete = _conns[it->fd].request.feed(buffer, _servers[0].getConfig()); // TODO: get correct config
                    // the config is just for max, is ok for now
                    if (!complete)
                        continue;
                        
                    // _conns[it->fd].buffer.append(buffer, 0, res);
                    // _conns[it->fd].contentLength += res;

                    Logger::debug("extended buffer");
                    // shutdown(it->fd, 0); // no reads anymore
                    
                    std::cout << _conns[it->fd].buffer << std::endl;
                    
                    // HttpRequest request(_conns[it->fd].buffer);
                    Server &s = determineServer(_conns[it->fd].request, _conns[it->fd].port);
                    // request.parse(s.getConfig());
                    

                    // set socket timeout if keep-alive is set
                    std::string header_keepalive = _conns[it->fd].request.getHeader("Keep-Alive");
                    if (!header_keepalive.empty()) {
                        size_t pos = header_keepalive.find("Timeout=");
                        header_keepalive.erase(pos, std::string("Timeout=").length());
                        Logger::debug("Header 'Keep-Alive: " + header_keepalive + "'");

                        struct timeval sock_timeout;
                        sock_timeout.tv_usec = 0;
                        try {
                            sock_timeout.tv_sec = stringToInt(header_keepalive);

                            // ensure valid value
                            if (sock_timeout.tv_sec <= 0)
                                throw HttpRequestException(400);
                            if (sock_timeout.tv_sec > MAX_KEEPALIVE)
                                sock_timeout.tv_sec = MAX_KEEPALIVE;
                            Logger::debug("keepalive is " + intToString(sock_timeout.tv_sec));
                            _conns[it->fd].keepalive = true;
                        } catch(std::exception &e) {
                            throw HttpRequestException(400);
                        }
                        int optval = 0;
                        setsockopt(it->fd, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval));
                        setsockopt(it->fd, SOL_SOCKET, SO_RCVTIMEO, &sock_timeout, sizeof(sock_timeout));
                        setsockopt(it->fd, SOL_SOCKET, SO_SNDTIMEO, &sock_timeout, sizeof(sock_timeout));
                    }
                    
                    // figure out where to direct request
                    // Logger::info(intToString(_conns[it->fd].port));
                    // Server &s = determineServer(request, _conns[it->fd].port);
                    HttpResponse response = s.handleResponse(&_conns[it->fd].request);
                    // Logger::info(request.getMethod() + " " + request.getURI());
                    std::string responseStr = response.getResponse();
                    if (send(it->fd, responseStr.c_str(), responseStr.size(), 0) < 0) {
                        Logger::error();
                        perror("error3: ");
                    }
                    close(it->fd);
                    _conns.erase(it->fd);
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
