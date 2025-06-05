#include <gtest/gtest.h>
#include <Webserv.hpp>

int main(int argc, char *argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

TEST(socket_handler, basic) {
	SocketHandler sh;	
}

TEST(servers, single_integration) {
	Config& config = Config::instance();
    config.parse("conf/default.yml");

    config_array servers = config.getServers();
	Server s(servers.at(0).getMap());
}
