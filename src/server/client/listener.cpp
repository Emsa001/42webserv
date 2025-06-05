#include "Webserv.hpp"

bool Server::isStop(){
    return g_stop;
}

// void Server::listener(int server_sock) {
//     listen(server_sock, MAX_CLIENTS);
//     this->setNonBlocking(server_sock);

//     pollfd fd;
//     fd.fd = server_sock;
//     fd.events = POLLIN;
//     fd.revents = 0;

//     fds.push_back(fd);
//     const int timeout_ms = 1000;

//     Logger::success("Server: " + this->getServerName() + " started listening on port " + this->config.at("listen").getString());

//     // while (!this->isStop()) {

//     //     int ret = poll(fds.data(), fds.size(), timeout_ms);
//     //     if (ret < 0) {
//     //         perror("poll");
//     //         break;
//     //     }

//     //     for (size_t i = 0; i < fds.size(); ++i) {
//     //         if (fds[i].fd == server_sock && (fds[i].revents & POLLIN)) {
//     //             this->acceptNewConnections(server_sock);
//     //         } else if (fds[i].revents & POLLIN) {
//     //             this->handleClientRead(i);
//     //         }
//     //     }

//     //     this->checkIdleClients();
//     // }

//     // Logger::warning("Server: " + this->getServerName() + " stopped");
// }
