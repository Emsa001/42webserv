#include "Webserv.hpp"
#include "SocketHandler.hpp"

SocketHandler::SocketHandler(config_array& servers) : _servers(servers) {
    std::cout << "SocketHandler()" << std::endl;
}

SocketHandler::SocketHandler() {
    Config& config = Config::instance();
    config.parse("conf/default.yml");
    _servers = config.getServers();
}

SocketHandler::~SocketHandler() {
    std::cout << "~SocketHandler" << std::endl;
}

int SocketHandler::run() {
    2
}