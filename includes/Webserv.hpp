#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#define PROJECT_NAME "webserv"

#define DEFAULT_MAX_HEADER_SIZE 8192
#define DEFAULT_MAX_BODY_SIZE 8192
#define REQUEST_SIZE_LIMIT 1024*1024*1024
#define READ_BUFFER_SIZE 10*1024
#define HARD_BUFFER_LIMIT 10 * 1024 * 1024 // 10 MB
#define MAX_CLIENTS 10
#define ROOT_DIR "./www/"
#define ALLOWED_METHODS "GET POST DELETE"
#define DEFAULT_SOCKET_TIMEOUT 30
#define CGI_TIMEOUT 3

#include "Colors.hpp"

// system headers
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h> 
#include <sys/time.h>
#include <netdb.h>

// network headers (for TCP)
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

// c headers
#include <unistd.h>
#include <fcntl.h>
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
#include "Logger.hpp"
#include "SocketHandler.hpp"


void signalHandler(int signum);
extern bool stop;

#endif

