#include "Webserv.hpp"

// volatile sig_atomic_t g_stop = 0;

void* startServer(void* arg) {
    config_map* data = static_cast<config_map*>(arg);
    Server server(*data);
    std::cout << std::endl;
    // server.start();
    std::cout << std::endl;
    return NULL;
}

int main()
{
    Config &config = Config::instance();
    config.parse("conf/default.yml");
    Server server(config.getServers()[0].getMap());
    

    Logger::init();
	SocketHandler sh(config.getServers());
	sh.run();
    // TODO: replace threads
    // TODO: initialize servers
    Logger::destroy();

    return 0;
}
