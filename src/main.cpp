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
        config.parse("conf/default.yml");

        Server server(config.getServers()[0].getMap());
        
        int max_client_body_size = Config::getSafe(server.getConfig(), "max_client_body_size", ConfigValue(1000000)).getInt();
        std::cout << "Max client body size: " << max_client_body_size << std::endl;

        // Logger::init();
        // SocketHandler sh(config.getServers());
        // sh.run();
        // Logger::destroy();

        return 0;
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}
