#include "Webserv.hpp"

int main()
{
	Config &config = Config::instance();
	config.parse("conf/default.yml");
	Server server(config.getServers()[0].getMap());

	HttpRequest request(
		"GET // HTTP/1.1\r\n"
		"Host: MainServer\r\n"
		"Connection: close\r\n\r\n",
		server.getConfig()
	);

	HttpResponse response = server.handleResponse(&request);
	std::cout << "Request Method: " << request.getMethod() << std::endl;
	std::cout << "Request URL: " << request.getURL()->getPath() << std::endl;

	std::cout << "Response Status Line: " << response.getStatusLine() << std::endl;
	std::cout << "Response: " << response.getBody() << std::endl;

	return 0;
}
