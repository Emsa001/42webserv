#include <gtest/gtest.h>
#include <Webserv.hpp>

TEST(WebservTests, BasicGetRootMainServer)
{
	Config &config = Config::instance();
	config.parse("conf/default.yml");
	Server server(config.getServers()[0].getMap());

	HttpRequest request(
		"GET / HTTP/1.1\r\n"
		"Host: MainServer\r\n"
		"Connection: close\r\n\r\n",
		server.getConfig()
	);

	HttpResponse response = server.handleResponse(&request);

	EXPECT_FALSE(response.isListing());
	EXPECT_EQ(response.getBody().length(), stringToInt(response.getHeader("Content-Length")));
	EXPECT_TRUE(response.getHeader("Connection") == "close");
	EXPECT_EQ(response.getStatusCode(), 200);
}

TEST(WebservTests, NotFoundUnknownPath)
{
	Config &config = Config::instance();
	config.parse("conf/default.yml");
	Server server(config.getServers()[2].getMap());

	HttpRequest request(
		"GET /unknown HTTP/1.1\r\n"
		"Host: MyServer2\r\n"
		"Connection: close\r\n\r\n",
		server.getConfig()
	);

	HttpResponse response = server.handleResponse(&request);

	EXPECT_FALSE(response.isListing());
	EXPECT_EQ(response.getStatusCode(), 404);
}

TEST(WebservTests, AutoindexPublicDirectory)
{
	Config &config = Config::instance();
	config.parse("conf/default.yml");
	Server server(config.getServers()[2].getMap());

	HttpRequest request(
		"GET /public HTTP/1.1\r\n"
		"Host: MyServer2\r\n"
		"Connection: close\r\n\r\n",
		server.getConfig()
	);

	HttpResponse response = server.handleResponse(&request);

	EXPECT_EQ(response.getStatusCode(), 200);
	EXPECT_TRUE(response.isListing());
	EXPECT_TRUE(response.getBody().find("Index of") != std::string::npos); // assuming autoindex is HTML
}

TEST(WebservTests, CgiScriptExecution)
{
	Config &config = Config::instance();
	config.parse("conf/default.yml");
	Server server(config.getServers()[3].getMap());

	HttpRequest request(
		"GET /cgi HTTP/1.1\r\n"
		"Host: MyServer3\r\n"
		"Connection: close\r\n\r\n",
		server.getConfig()
	);

	HttpResponse response = server.handleResponse(&request);

	EXPECT_EQ(response.getStatusCode(), 200);
	EXPECT_TRUE(response.isCgi());
	EXPECT_TRUE(response.getBody().find("{\"status\": \"success\", \"files\": []}") != std::string::npos);
}

TEST(WebservTests, RequestBodyTooLarge)
{
	Config &config = Config::instance();
	config.parse("conf/default.yml");
	Server server(config.getServers()[3].getMap());

	std::string largeBody(16385, 'A'); // exceeds MyServer3's 16384 limit

	HttpRequest request(
		"POST /cgi HTTP/1.1\r\n"
		"Host: MyServer3\r\n"
		"Content-Length: " + std::to_string(largeBody.size()) + "\r\n"
		"Connection: close\r\n\r\n" +
		largeBody,
		server.getConfig()
	);

	HttpResponse response = server.handleResponse(&request);
	EXPECT_EQ(response.getStatusCode(), 413); // 413 Payload Too Large
}

TEST(WebservTests, MethodNotAllowed)
{
	Config &config = Config::instance();
	config.parse("conf/default.yml");
	Server server(config.getServers()[1].getMap());

	HttpRequest request(
		"POST / HTTP/1.1\r\n"
		"Host: MyServer1\r\n"
		"Content-Length: 0\r\n"
		"Connection: close\r\n\r\n",
		server.getConfig()
	);

	HttpResponse response = server.handleResponse(&request);

	EXPECT_EQ(response.getStatusCode(), 405); // Method Not Allowed
}

TEST(WebservTests, IndexHtmlServed)
{
	Config &config = Config::instance();
	config.parse("conf/default.yml");
	Server server(config.getServers()[0].getMap());

	for(int i = 2; i <= 5; i++){
		std::string path = "/" + intToString(i);

		std::string requestStr =
			"GET " + path + " HTTP/1.1\r\n"
			"Host: MainServer\r\n"
			"Connection: close\r\n\r\n";
	
		HttpRequest request(requestStr, server.getConfig());
		HttpResponse response = server.handleResponse(&request);

		const FileData fileData = response.getFileData();
		const config_map *locationData = response.getLocationData();
		std::string index = Config::getSafe(*locationData, "index", ConfigValue("index.html")).getString();
	
		EXPECT_EQ(index, "index" + intToString(i) + ".html");
	}
}
