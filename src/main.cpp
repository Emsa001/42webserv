#include "Webserv.hpp"

// volatile sig_atomic_t g_stop = 0;

void* startServer(void* arg) {
    config_map* data = static_cast<config_map*>(arg);
    Server server(*data);
    std::cout << std::endl;
    server.start();
    std::cout << std::endl;
    return NULL;
}

int main()
{
	Config &config = Config::instance();
	config.parse("conf/default.yml");
	// Server server(config.getServers()[0].getMap());

	// HttpRequest request(
	// 	"GET // HTTP/1.1\r\n"
	// 	"Host: MainServer\r\n"
	// 	"Connection: close\r\n\r\n",
	// 	server.getConfig()
	// );

	// HttpResponse response = server.handleResponse(&request);
	// std::cout << "Request Method: " << request.getMethod() << std::endl;
	// std::cout << "Request URL: " << request.getURL()->getPath() << std::endl;

	// std::cout << "Response Status Line: " << response.getStatusLine() << std::endl;
	// std::cout << "Response: " << response.getBody() << std::endl;

	SocketHandler sh(config.getServers());
	sh.run();

	return 0;
    //TODO: replace threads

    // Server s(it->getMap());
    // s.start();
    SocketHandler sh;
    sh.run();
    // TODO: initialize servers

    Logger::destroy();

    return 0;
}
