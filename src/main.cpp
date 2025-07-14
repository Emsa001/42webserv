
#include "Webserv.hpp"

bool stop =false;

int main()
{
    try
    {
        signal(SIGINT, signalHandler);
        Config &config = Config::instance();
        config.parse("conf/default.yml");

        Logger::init();
        SocketHandler sh(config.getServers());
        int status = sh.run();
        Logger::destroy();

        return status;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}
