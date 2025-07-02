#include <gtest/gtest.h>
#include <Webserv.hpp>

TEST(WebservCookiesTests, BasicCookieTest)
{
    Config &config = Config::instance();
    config.parse("conf/default.yml");
    Server server(config.getServers()[0].getMap());

    HttpRequest request(
        "GET / HTTP/1.1\r\n"
        "Host: MainServer\r\n"
        "Cookie: test_cookie=12345\r\n"
        "Connection: close\r\n\r\n",
        server.getConfig()
    );

    HttpResponse response = server.handleResponse(&request);

    // Verify status
    EXPECT_EQ(response.getStatusCode(), 200);
    EXPECT_TRUE(response.getStatusLine().find("200 OK") != std::string::npos);

    // Ensure the cookie did not change (no Set-Cookie in response)
    std::string raw = response.getResponse();
    EXPECT_TRUE(raw.find("Set-Cookie:") == std::string::npos);

    // Optionally, check if body contains some static content (e.g., index.html)
    EXPECT_TRUE(raw.find("<html") != std::string::npos || raw.find("<!DOCTYPE") != std::string::npos);
}
