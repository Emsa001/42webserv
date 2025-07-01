#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#define PROJECT_NAME "webserv"

#define DEFAULT_MAX_HEADER_SIZE 8192
#define DEFAULT_MAX_BODY_SIZE 8192
#define READ_BUFFER_SIZE 4096
#define HARD_BUFFER_LIMIT 10 * 1024 * 1024 // 10 MB
#define MAX_CLIENTS 10
#define ROOT_DIR "./www/"
#define DEFAULT_METHODS "GET POST DELETE"

#include "Colors.hpp"

// system headers
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h> 
#include <netdb.h>

// network headers (for TCP)
#include <netinet/in.h>
#include <netinet/tcp.h>

// c headers
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <poll.h>

// c++ headers
#include <iostream>
#include <sstream>
#include <cstring>
#include <algorithm>
#include <fstream>
#include <ctime>
#include <csignal>

#include <vector>
#include <map>
#include <stack>
#include <string>
#include <cstdlib>
#include <list>
#include <dirent.h>

#include "Aliases.hpp"

// project headers
#include "Utils.hpp"
#include "Logger.hpp"
#include "Config.hpp"
#include "Server.hpp"
#include "SocketHandler.hpp"
#include "Cgi.hpp"
#include "HttpRequest.hpp"


void signalHandler(int signum);

// #include <iostream> // → Allows us to use std::cout and std::cerr for logging.
// #include <fstream> // → Used to read files (to serve static files).
// #include <sstream> // → Used for string manipulation (converting HTTP responses to strings).
// #include <string> // → Allows us to use std::string for text handling.
// #include <vector> // → Used for storing multiple sockets in poll().
// #include <cstring> // → Used for memset() to clear data structures.
// #include <sys/types.h> // → Required for networking functions.
// #include <sys/socket.h> // → Provides functions like socket(), bind(), listen(), accept(), etc.
// #include <netinet/in.h> // → Contains sockaddr_in (for IP addresses and ports).
// #include <unistd.h> // → Provides close() function for sockets.
// #include <fcntl.h> // → Used to set sockets as non-blocking.
// #include <poll.h> // → Allows us to use poll() for monitoring multiple clients.

#endif

