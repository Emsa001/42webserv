#include <netdb.h>

#include "Webserv.hpp"

// int Server::start()
// {
//     struct addrinfo hints, *res;
//     int status;

//     memset(&hints, 0, sizeof hints);
//     hints.ai_family = AF_INET;       // IPv4
//     hints.ai_socktype = SOCK_STREAM; // TCP
//     hints.ai_flags = AI_PASSIVE;     // Use my IP

//     std::string host = Config::getSafe(*this->config, "host",
//     ConfigValue((std::string) "127.0.0.1")).getString(); const std::string
//     &port = this->config->at("listen");

//     if ((status = getaddrinfo(host.c_str(), port.c_str(), &hints, &res)) !=
//     0)
//     {
//         std::cerr << "getaddrinfo: " << gai_strerror(status) << std::endl;
//         throw std::runtime_error("Failed to create socket");

//     }

//     struct pollfd newfd = (struct pollfd){ .events = POLLIN, .revents = 0 };

//     newfd.events = POLLIN;
//     newfd.revents = 0;
//     newfd.fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

//     if (newfd.fd < 0)
//     {
//         std::cerr << "Failed to create socket" << std::endl;
//         freeaddrinfo(res);
//         throw std::runtime_error("Failed to create socket");
//     }

//     int opt = 1;
//     setsockopt(newfd.fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

//     if (bind(newfd.fd, res->ai_addr, res->ai_addrlen) < 0)
//     {
//         std::cerr << "Bind failed" << std::endl;
//         freeaddrinfo(res);
//         close(newfd.fd);
//         throw std::runtime_error("Failed to create socket");

//     }

//     this->setNonBlocking(newfd.fd);
//     listen(newfd.fd, MAX_CLIENTS);

//     this->listener(newfd.fd);

//     // cleaning
//     freeaddrinfo(res);
//     close(newfd.fd);
//     return 0;
// }
