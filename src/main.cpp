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
    try {
        Config &config = Config::instance();
        config.parse("conf/virtual_hosts_example.yml");
        
        Logger::init();
        SocketHandler sh(config.getServers());
        sh.run();
        Logger::destroy();

        return 0;
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}
