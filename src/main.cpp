
#include "Webserv.hpp"

bool stop =false;

int main(int argc, char *argv[])
{
    try
    {
        signal(SIGINT, signalHandler);
        Config &config = Config::instance();
        std::string fn = "conf/default.yml";
        if (argc >= 2)
            fn = argv[1];
        config.parse(fn);

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
