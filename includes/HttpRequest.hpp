#ifndef HTTP_REQUEST_HPP
#define HTTP_REQUEST_HPP

#include "Webserv.hpp"
#include "HttpMessage.hpp"

class HttpURL {
    private:
        std::string path;
        std::string query;
        StringMap queryMap;

    public:
        HttpURL() : path(""), query("") {}
        HttpURL(const std::string &url);
        ~HttpURL() {}

        const std::string &getPath() const { return path; }
        const std::string &getQuery() const { return query; }
        const StringMap &getQueryMap() const { return queryMap; }
};

class HttpRequest : public HttpMessage {
    private:
        HttpURL url;
        std::string method;
        std::string uri;
        std::string version;
        std::string port;

        
        std::string rawRequestData;
        bool headersComplete;
        bool bodyComplete;
        std::string::size_type headerEnd;

        int content_length;

        std::string normalizeUri(const std::string &uri);


    public:
        HttpRequest(const std::string &rawData="") : rawRequestData(rawData), headersComplete(false), bodyComplete(false),
            headerEnd(std::string::npos), content_length(-1) {
        }

        ~HttpRequest() { }

        void parseHeaders(const config_map &serverConfig);
        void parse(const config_map &serverConfig, const config_map &location);

        bool feed(const std::string & addition, size_t len, const config_map &serverConfig);

        // --- Setters ---
        void setMethod(const std::string &m) { method = m; }
        void setUri(const std::string &u) { uri = u; }
        void setVersion(const std::string &v) { version = v; }

        // --- Getters ---
        const std::string &getMethod() const { return method; }
        const std::string &getURI() const { return uri; }
        const std::string &getVersion() const { return version; }
        HttpURL getURL() const { return url; }
        const std::string &getPort() const { return port; }
        const std::string &getRawRequestData() const { return rawRequestData; }
        int getBodyLength() { return body.size(); }

        
        bool getHeadersComplete() const { return headersComplete; }
        bool getBodyComplete() const { return bodyComplete; }
};

class HttpRequestException : public std::exception {
    private:
        int statusCode;

    public:
        explicit HttpRequestException(int statusCode) : statusCode(statusCode) {}
        int getStatusCode() const { return statusCode; }
};


struct ClientRequestState {
    // connection data
    int port;
    bool keepalive;
    Server *server;
    // request data
    HttpRequest request;
    std::string response;

    std::string buffer;
    bool headersComplete;
    size_t contentLength;
    size_t expectedSize;
};

#endif
