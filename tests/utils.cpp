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

TEST(WebservUtilsTests, toLower) {
    std::string s = "TeSt";
    s =  toLower(s);
    EXPECT_STREQ(s.c_str(), "test");
}