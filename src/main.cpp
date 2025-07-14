
#include "Webserv.hpp"

bool stop =false;

int main()
{
    try
    {
        signal(SIGINT, signalHandler);
        Config &config = Config::instance();
        config.parse("conf/tester.yml");

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
