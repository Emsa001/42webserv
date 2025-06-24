#include <gtest/gtest.h>
#include <Webserv.hpp>

/** 
 * 
 * Webserv server's response handling tests.
 * All Requests are assumed to be valid. HttpResponse works only with already fully checked HttpRequest objects.
 * Request is parsed in server.handleResponse() method.
 * 
 * The tests cover:
 * - Basic GET request to the root of the main server
 * - Handling of unknown paths (404 Not Found)
 * - Autoindexing of a public directory
 * - Execution of CGI scripts
 * - Handling of request bodies that exceed size limits (413 Payload Too Large)
 * - Handling of request headers that exceed size limits (431 Request Header Fields Too Large)
 * - Method not allowed responses (405 Method Not Allowed)
 * - Index location handling for different paths
 * 

*/

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
	EXPECT_TRUE(response.getBody().find("Index of") != std::string::npos);
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

TEST(WebservTests, RequestHeaderTooLarge)
{
	Config &config = Config::instance();
	config.parse("conf/default.yml");
	Server server(config.getServers()[0].getMap()); // MainServer

	// Create a header that exceeds 1024 bytes
	std::string largeHeader = "GET / HTTP/1.1\r\nHost: MainServer\r\n";
	while (largeHeader.size() < 1100)
		largeHeader += "X-Dummy-Header: value\r\n";
	largeHeader += "\r\n";

	HttpRequest request(largeHeader, server.getConfig());
	HttpResponse response = server.handleResponse(&request);

	EXPECT_EQ(response.getStatusCode(), 431); // 431 Request Header Fields Too Large
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

TEST(WebservTests, IndexLocation)
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
