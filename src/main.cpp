#include "Webserv.hpp"

int main()
{
    try
    {
        Config &config = Config::instance();
        config.parse("conf/default.yml");

        Logger::init();
        SocketHandler sh(config.getServers());
        sh.run();
        Logger::destroy();

        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}
