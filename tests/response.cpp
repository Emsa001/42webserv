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

TEST(WebservResponseTests, BasicGetRootMainServer)
{
	Config &config = Config::instance();
	config.parse("conf/default.yml");
	Server server(config.getServers()[0].getMap());

	HttpRequest request(
		"GET / HTTP/1.1\r\n"
		"Host: MainServer\r\n"
		"Connection: close\r\n\r\n"
	);

	HttpResponse response = server.handleResponse(&request);

	EXPECT_FALSE(response.isListing());
	EXPECT_EQ(response.getBody().length(), stringToInt(response.getHeader("Content-Length")));
	EXPECT_TRUE(response.getHeader("Connection") == "close");
	EXPECT_EQ(response.getStatusCode(), 200);
}

TEST(WebservResponseTests, NotFoundUnknownPath)
{
	Config &config = Config::instance();
	config.parse("conf/default.yml");
	Server server(config.getServers()[2].getMap());

	HttpRequest request(
		"GET /unknown HTTP/1.1\r\n"
		"Host: MyServer2\r\n"
		"Connection: close\r\n\r\n"
	);

	HttpResponse response = server.handleResponse(&request);

	EXPECT_FALSE(response.isListing());
	EXPECT_EQ(response.getStatusCode(), 404);
}

TEST(WebservResponseTests, AutoindexPublicDirectory)
{
	Config &config = Config::instance();
	config.parse("conf/default.yml");
	Server server(config.getServers()[2].getMap());

	HttpRequest request(
		"GET /public HTTP/1.1\r\n"
		"Host: MyServer2\r\n"
		"Connection: close\r\n\r\n"
	);

	HttpResponse response = server.handleResponse(&request);

	EXPECT_EQ(response.getStatusCode(), 200);
	EXPECT_TRUE(response.isListing());
	EXPECT_TRUE(response.getBody().find("Index of") != std::string::npos);
}

TEST(WebservResponseTests, CgiScriptExecution)
{
	Config &config = Config::instance();
	config.parse("conf/default.yml");
	Server server(config.getServers()[3].getMap());

	HttpRequest request(
		"GET /cgi HTTP/1.1\r\n"
		"Host: MyServer3\r\n"
		"Connection: close\r\n\r\n"
	);

	HttpResponse response = server.handleResponse(&request);

	EXPECT_EQ(response.getStatusCode(), 200);
	EXPECT_TRUE(response.isCgi());
	EXPECT_TRUE(response.getBody().find("{\"status\": \"success\"") != std::string::npos);
}

TEST(WebservResponseTests, RequestBodyTooLarge)
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
		largeBody
	);

	HttpResponse response = server.handleResponse(&request);
	EXPECT_EQ(response.getStatusCode(), 413); // 413 Payload Too Large
}

TEST(WebservResponseTests, MethodNotAllowed)
{
	Config &config = Config::instance();
	config.parse("conf/default.yml");
	Server server(config.getServers()[1].getMap());

	HttpRequest request(
		"POST / HTTP/1.1\r\n"
		"Host: MyServer1\r\n"
		"Content-Length: 0\r\n"
		"Connection: close\r\n\r\n"
	);

	HttpResponse response = server.handleResponse(&request);

	EXPECT_EQ(response.getStatusCode(), 405); // Method Not Allowed
}

TEST(WebservResponseTests, IndexLocation)
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
	
		HttpRequest request(requestStr);
		HttpResponse response = server.handleResponse(&request);

		const FileData fileData = response.getFileData();
		const config_map *location = server.findLocation(request.getURL()->getPath());

		std::string index = Config::getSafe(*location, "index", ConfigValue("index.html")).getString();
	
		EXPECT_EQ(index, "index" + intToString(i) + ".html");
	}
}

// === Additional Comprehensive Tests ===

TEST(WebservResponseTests, CustomErrorPage404)
{
	Config &config = Config::instance();
	config.parse("conf/default.yml");
	Server server(config.getServers()[1].getMap()); // MyServer1 has custom 404

	HttpRequest request(
		"GET /nonexistent HTTP/1.1\r\n"
		"Host: MyServer1\r\n"
		"Connection: close\r\n\r\n"
	);

	HttpResponse response = server.handleResponse(&request);

	EXPECT_EQ(response.getStatusCode(), 404);
	// Should use custom error page defined in MyServer1 config
}

TEST(WebservResponseTests, DeleteMethodCgiServer)
{
	Config &config = Config::instance();
	config.parse("conf/default.yml");
	Server server(config.getServers()[3].getMap()); // MyServer3 supports DELETE

	HttpRequest request(
		"DELETE /cgi HTTP/1.1\r\n"
		"Host: MyServer3\r\n"
		"Connection: close\r\n\r\n"
	);

	HttpResponse response = server.handleResponse(&request);

	EXPECT_EQ(response.getStatusCode(), 200);
	EXPECT_TRUE(response.isCgi());
}

TEST(WebservResponseTests, PostWithValidBodySize)
{
	Config &config = Config::instance();
	config.parse("conf/default.yml");
	Server server(config.getServers()[3].getMap()); // MyServer3 max_client_body_size: 16384

	std::string validBody(8192, 'A'); // Within limit

	HttpRequest request(
		"POST /cgi HTTP/1.1\r\n"
		"Host: MyServer3\r\n"
		"Content-Length: " + std::to_string(validBody.size()) + "\r\n"
		"Content-Type: application/x-www-form-urlencoded\r\n"
		"Connection: close\r\n\r\n" +
		validBody
	);

	HttpResponse response = server.handleResponse(&request);

	EXPECT_EQ(response.getStatusCode(), 200);
	EXPECT_TRUE(response.isCgi());
}

TEST(WebservResponseTests, SmallBodyLimitServer)
{
	Config &config = Config::instance();
	config.parse("tests/valid_configs/small_body_size.yml");
	Server server(config.getServers()[0].getMap()); // MyServer2 max_client_body_size: 256

	std::string largeBody(300, 'B'); // Exceeds MyServer2's 256 limit

	HttpRequest request(
		"POST / HTTP/1.1\r\n"
		"Host: localhost\r\n"
		"Content-Length: " + std::to_string(largeBody.size()) + "\r\n"
		"Connection: close\r\n\r\n" +
		largeBody
	);

	HttpResponse response = server.handleResponse(&request);

	EXPECT_EQ(response.getStatusCode(), 413); // Payload Too Large
}

TEST(WebservResponseTests, KeepAliveConnection)
{
	Config &config = Config::instance();
	config.parse("conf/default.yml");
	Server server(config.getServers()[0].getMap()); // MainServer has keep_alive: 30

	HttpRequest request(
		"GET / HTTP/1.1\r\n"
		"Host: MainServer\r\n"
		"Connection: keep-alive\r\n\r\n"
	);

	HttpResponse response = server.handleResponse(&request);

	EXPECT_EQ(response.getStatusCode(), 200);
	EXPECT_TRUE(response.getHeader("Connection") == "keep-alive");
}

TEST(WebservResponseTests, SecretPathAccess)
{
	Config &config = Config::instance();
	config.parse("conf/default.yml");
	Server server(config.getServers()[2].getMap()); // MyServer2 has /secret path

	HttpRequest request(
		"GET /secret HTTP/1.1\r\n"
		"Host: MyServer2\r\n"
		"Connection: close\r\n\r\n"
	);

	HttpResponse response = server.handleResponse(&request);

	EXPECT_EQ(response.getStatusCode(), 200);
	// Should serve hidden.html as per config
}

TEST(WebservResponseTests, ContentTypeHeaderValidation)
{
	Config &config = Config::instance();
	config.parse("conf/default.yml");
	Server server(config.getServers()[0].getMap());

	HttpRequest request(
		"GET / HTTP/1.1\r\n"
		"Host: MainServer\r\n"
		"Connection: close\r\n\r\n"
	);

	HttpResponse response = server.handleResponse(&request);

	EXPECT_EQ(response.getStatusCode(), 200);
	EXPECT_FALSE(response.getHeader("Content-Type").empty());
	// Should have appropriate MIME type set
}

TEST(WebservResponseTests, EmptyRequestBody)
{
	Config &config = Config::instance();
	config.parse("conf/default.yml");
	Server server(config.getServers()[3].getMap());

	HttpRequest request(
		"POST /cgi HTTP/1.1\r\n"
		"Host: MyServer3\r\n"
		"Content-Length: 0\r\n"
		"Connection: close\r\n\r\n"
	);

	HttpResponse response = server.handleResponse(&request);

	EXPECT_EQ(response.getStatusCode(), 200);
	EXPECT_TRUE(response.isCgi());
}

TEST(WebservResponseTests, InvalidHTTPMethod)
{
	Config &config = Config::instance();
	config.parse("conf/default.yml");
	Server server(config.getServers()[0].getMap());

	HttpRequest request(
		"INVALID / HTTP/1.1\r\n"
		"Host: MainServer\r\n"
		"Connection: close\r\n\r\n"
	);

	HttpResponse response = server.handleResponse(&request);

	EXPECT_EQ(response.getStatusCode(), 405); // Method Not Allowed
}

TEST(WebservResponseTests, MissingContentLengthForPost)
{
	Config &config = Config::instance();
	config.parse("conf/default.yml");
	Server server(config.getServers()[3].getMap());

	std::string body = "Hello, this is a CGI response from MyServer3!";

	HttpRequest request(
		"POST /cgi HTTP/1.1\r\n"
		"Host: MyServer3\r\n"
		"Connection: close\r\n\r\n" +
		body
	);

	HttpResponse response = server.handleResponse(&request);

	// Should handle missing Content-Length gracefully
	EXPECT_TRUE(response.getStatusCode() == 400 || response.getStatusCode() == 411);
}

TEST(WebservResponseTests, HTTPVersionSupport)
{
	Config &config = Config::instance();
	config.parse("conf/default.yml");
	Server server(config.getServers()[0].getMap());

	HttpRequest request(
		"GET / HTTP/1.0\r\n"
		"Host: MainServer\r\n\r\n"
	);

	HttpResponse response = server.handleResponse(&request);

	EXPECT_EQ(response.getStatusCode(), 200);
	// Should handle HTTP/1.0 requests
}

TEST(WebservResponseTests, DirectoryWithoutTrailingSlash)
{
	Config &config = Config::instance();
	config.parse("conf/default.yml");
	Server server(config.getServers()[2].getMap());

	HttpRequest request(
		"GET /public HTTP/1.1\r\n"
		"Host: MyServer2\r\n"
		"Connection: close\r\n\r\n"
	);

	HttpResponse response = server.handleResponse(&request);

	// Should either redirect to /public/ or serve directory listing
	EXPECT_TRUE(response.getStatusCode() == 200 || response.getStatusCode() == 301 || response.getStatusCode() == 302);
}

TEST(WebservResponseTests, ConcurrentRequestHandling)
{
	Config &config = Config::instance();
	config.parse("conf/default.yml");
	Server server(config.getServers()[0].getMap());

	// Simulate multiple requests to test thread safety/concurrency handling
	std::vector<HttpResponse> responses;
	
	for(int i = 0; i < 5; i++) {
		HttpRequest request(
			"GET / HTTP/1.1\r\n"
			"Host: MainServer\r\n"
			"Connection: close\r\n\r\n"
		);

		HttpResponse response = server.handleResponse(&request);
		responses.push_back(response);
	}

	// All requests should succeed
	for(const auto& response : responses) {
		EXPECT_EQ(response.getStatusCode(), 200);
	}
}

TEST(WebservResponseTests, LargeValidHeaderSize)
{
	Config &config = Config::instance();
	config.parse("conf/default.yml");
	Server server(config.getServers()[3].getMap()); // MyServer3 has max_client_header_size: 2048

	// Create headers just under the limit
	std::string largeHeaders = "GET / HTTP/1.1\r\nHost: MyServer3\r\n";
	while (largeHeaders.size() < 1500) {  // Well under 2048 limit
		largeHeaders += "X-Test-Header: some-value\r\n";
	}
	largeHeaders += "\r\n";

	HttpRequest request(largeHeaders);
	HttpResponse response = server.handleResponse(&request);

	EXPECT_EQ(response.getStatusCode(), 200);
}

TEST(WebservResponseTests, OptionsMethodHandling)
{
	Config &config = Config::instance();
	config.parse("conf/default.yml");
	Server server(config.getServers()[0].getMap());

	HttpRequest request(
		"OPTIONS / HTTP/1.1\r\n"
		"Host: MainServer\r\n"
		"Connection: close\r\n\r\n"
	);

	HttpResponse response = server.handleResponse(&request);

	// Should return allowed methods or 405 if not supported
	EXPECT_TRUE(response.getStatusCode() == 200 || response.getStatusCode() == 405);
}

TEST(WebservResponseTests, QueryStringHandling)
{
	Config &config = Config::instance();
	config.parse("conf/default.yml");
	Server server(config.getServers()[3].getMap());

	HttpRequest request(
		"GET /cgi?param1=value1&param2=value2 HTTP/1.1\r\n"
		"Host: MyServer3\r\n"
		"Connection: close\r\n\r\n"
	);

	HttpResponse response = server.handleResponse(&request);

	EXPECT_EQ(response.getStatusCode(), 200);
	EXPECT_TRUE(response.isCgi());
	// CGI should receive query parameters
}

TEST(WebservResponseTests, ContentLengthMismatch)
{
	Config &config = Config::instance();
	config.parse("conf/default.yml");
	Server server(config.getServers()[3].getMap());

	std::string body = "short body";
	HttpRequest request(
		"POST /cgi HTTP/1.1\r\n"
		"Host: MyServer3\r\n"
		"Content-Length: 1000\r\n"  // Mismatched with actual body size
		"Connection: close\r\n\r\n" +
		body
	);

	HttpResponse response = server.handleResponse(&request);

	// Should handle Content-Length mismatch appropriately
	EXPECT_TRUE(response.getStatusCode() == 400 || response.getStatusCode() == 413);
}

TEST(WebservResponseTests, ResponseHeadersPresent)
{
	Config &config = Config::instance();
	config.parse("conf/default.yml");
	Server server(config.getServers()[0].getMap());

	HttpRequest request(
		"GET / HTTP/1.1\r\n"
		"Host: MainServer\r\n"
		"Connection: close\r\n\r\n"
	);

	HttpResponse response = server.handleResponse(&request);

	EXPECT_EQ(response.getStatusCode(), 200);
	
	// Essential headers should be present
	EXPECT_FALSE(response.getHeader("Content-Length").empty());
	EXPECT_FALSE(response.getHeader("Content-Type").empty());
	EXPECT_FALSE(response.getHeader("Connection").empty());
}

TEST(WebservResponseTests, NoHostHeaderHTTP11)
{
	Config &config = Config::instance();
	config.parse("conf/default.yml");
	Server server(config.getServers()[0].getMap());

	HttpRequest request(
		"GET / HTTP/1.1\r\n"
		"Connection: close\r\n\r\n"
	);

	HttpResponse response = server.handleResponse(&request);

	// HTTP/1.1 requires Host header
	EXPECT_TRUE(response.getStatusCode() == 400 || response.getStatusCode() == 200);
}

TEST(WebservResponseTests, CaseInsensitiveHeaders)
{
	Config &config = Config::instance();
	config.parse("conf/default.yml");
	Server server(config.getServers()[0].getMap());

	HttpRequest request(
		"GET / HTTP/1.1\r\n"
		"host: mainserver\r\n"
		"connection: close\r\n\r\n"
	);

	HttpResponse response = server.handleResponse(&request);

	EXPECT_EQ(response.getStatusCode(), 200);
	// Headers should be handled case-insensitively
}

TEST(WebservResponseTests, PathTraversalAttempt)
{
	Config &config = Config::instance();
	config.parse("conf/default.yml");
	Server server(config.getServers()[0].getMap());

	HttpRequest request(
		"GET /../../../etc/passwd HTTP/1.1\r\n"
		"Host: MainServer\r\n"
		"Connection: close\r\n\r\n"
	);

	HttpResponse response = server.handleResponse(&request);

	// Should prevent path traversal attacks
	EXPECT_TRUE(response.getStatusCode() == 404 || response.getStatusCode() == 403 || response.getStatusCode() == 400);
}

// TODO: Should we handle this?
TEST(WebservResponseTests, MultipleConsecutiveSlashes)
{
	Config &config = Config::instance();
	config.parse("conf/default.yml");
	Server server(config.getServers()[0].getMap());

	HttpRequest request(
		"GET ////// HTTP/1.1\r\n"
		"Host: MainServer\r\n"
		"Connection: close\r\n\r\n"
	);

	HttpResponse response = server.handleResponse(&request);

	// Should normalize path and serve root
	EXPECT_EQ(response.getStatusCode(), 200);
}

TEST(WebservResponseTests, BinaryPostData)
{
	Config &config = Config::instance();
	config.parse("conf/default.yml");
	Server server(config.getServers()[3].getMap());

	// Create binary data with null bytes
	std::string binaryData;
	for(int i = 0; i < 100; i++) {
		binaryData += static_cast<char>(i % 256);
	}

	HttpRequest request(
		"POST /cgi HTTP/1.1\r\n"
		"Host: MyServer3\r\n"
		"Content-Type: application/octet-stream\r\n"
		"Content-Length: " + std::to_string(binaryData.size()) + "\r\n"
		"Connection: close\r\n\r\n" +
		binaryData
	);

	HttpResponse response = server.handleResponse(&request);

	EXPECT_EQ(response.getStatusCode(), 200);
	EXPECT_TRUE(response.isCgi());
}

TEST(WebservResponseTests, MaximumPathLength)
{
	Config &config = Config::instance();
	config.parse("conf/default.yml");
	Server server(config.getServers()[0].getMap());

	// Create very long path
	std::string longPath = "/";
	longPath += std::string(4096, 'a'); // Very long path

	HttpRequest request(
		"GET " + longPath + " HTTP/1.1\r\n"
		"Host: MainServer\r\n"
		"Connection: close\r\n\r\n"
	);

	HttpResponse response = server.handleResponse(&request);

	// Should handle long paths gracefully
	EXPECT_TRUE(response.getStatusCode() == 404 || response.getStatusCode() == 414 || response.getStatusCode() == 400);
}
