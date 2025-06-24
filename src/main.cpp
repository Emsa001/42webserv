#include "Webserv.hpp"

int main()
{
	Config &config = Config::instance();
	config.parse("conf/default.yml");
	Server server(config.getServers()[0].getMap());

	for(int i = 1; i <= 5; i++){
		std::string path = (i > 1 ? "/" + intToString(i) : "/");

		std::string requestStr =
			"GET " + path + " HTTP/1.1\r\n"
			"Host: MainServer\r\n"
			"Connection: close\r\n\r\n";
	
		HttpRequest request(requestStr, server.getConfig());
		HttpResponse response = server.handleResponse(&request);

		const FileData fileData = response.getFileData();
		const config_map *locationData = response.getLocationData();
		std::string index = Config::getSafe(*locationData, "index", ConfigValue("index.html")).getString();

		std::cout << "Path: " << path << std::endl;
		std::cout << "name: " << fileData.path << std::endl;
		std::cout << "index: " << index << std::endl;
	
		// EXPECT_EQ(response.getStatusCode(), 200);
		// EXPECT_TRUE(response.getFileData().name == "index.html");
	}

	return 0;
}
