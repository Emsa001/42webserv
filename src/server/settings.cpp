#include "Server.hpp"

/*
    Function to set a socket as non-blocking. This is useful for handling
   multiple clients.

    - In blocking mode, accept(), recv(), and send() will stop the program if no
   data is available.

    - In non-blocking mode, the program continues running even if there’s no
   incoming connection or data.
*/

// TODO: Move this to SocketHandler

// void Server::setNonBlocking(int sock) {
//     int flags = fcntl(sock, F_GETFL, 0);
//     fcntl(sock, F_SETFL, flags | O_NONBLOCK);

//     // Enable TCP Keepalive
//     int optval = 1;
//     setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval));

//     int keep_idle = 10;     // Start checking after 10 second of inactivity
//     int keep_interval = 5;  // Send keep-alive probes every 5 second
//     int keep_count = 3;     // Disconnect after 3 failed probes

//     setsockopt(sock, IPPROTO_TCP, TCP_KEEPIDLE, &keep_idle,
//     sizeof(keep_idle)); setsockopt(sock, IPPROTO_TCP, TCP_KEEPINTVL,
//     &keep_interval, sizeof(keep_interval)); setsockopt(sock, IPPROTO_TCP,
//     TCP_KEEPCNT, &keep_count, sizeof(keep_count));
// }
