#include "Webserv.hpp"

// void Server::acceptNewConnections(int server_sock) {
//     while (true) {
//         sockaddr_in client_addr;
//         socklen_t client_len = sizeof(client_addr);
//         int client_fd = accept(server_sock, (sockaddr*)&client_addr, &client_len);

//         if (client_fd < 0) {
//             if (errno == EWOULDBLOCK || errno == EAGAIN) break;
//             perror("accept");
//             return;
//         }

//         Logger::clientConnect(client_fd);
//         this->setNonBlocking(client_fd);

//         pollfd fd;
//         fd.fd = client_fd;
//         fd.events = POLLIN;
//         fd.revents = 0;
//         fds.push_back(fd);

//         client_timestamps[client_fd] = time(NULL);
//     }
// }

// void Server::removeClient(size_t index) {
//     int fd = fds[index].fd;
//     int fd_copy = fd;

//     this->closeConnection(&fd);
//     fds.erase(fds.begin() + index);
//     client_timestamps.erase(fd_copy);
//     requestStates.erase(fd_copy);
// }

// void Server::closeConnection(int* client_sock) {
//     if (client_sock && *client_sock >= 0) {
//         Logger::clientDisconnect(*client_sock);
//         close(*client_sock);
//         *client_sock = -1;
//     }
// }
