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
        HttpURL(const std::string &url);
        ~HttpURL() {}

        const std::string &getPath() const { return path; }
        const std::string &getQuery() const { return query; }
        const StringMap &getQueryMap() const { return queryMap; }
};

class HttpRequest : public HttpMessage {
    private:
        HttpURL *url;
        std::string method;
        std::string uri;
        std::string version;
        std::string port;

        
        std::string rawRequestData;
        bool headersParsed;
        std::string::size_type headerEnd;
        int content_length;

        std::string normalizeUri(const std::string &uri);


    public:
        HttpRequest(const std::string &rawData="")
            : url(NULL), rawRequestData(rawData), headersParsed(false),
            headerEnd(std::string::npos), content_length(-1) {
        }

        ~HttpRequest() { delete url; }

        void parseHeaders(const config_map &serverConfig);
        void parse(const config_map &serverConfig);

        bool feed(const std::string & addition, size_t len, const config_map &serverConfig);

        // --- Setters ---
        void setMethod(const std::string &m) { method = m; }
        void setUri(const std::string &u) { uri = u; }
        void setVersion(const std::string &v) { version = v; }

        // --- Getters ---
        const std::string &getMethod() const { return method; }
        const std::string &getURI() const { return uri; }
        const std::string &getVersion() const { return version; }
        HttpURL *getURL() const { return url; }
        const std::string &getPort() const { return port; }
        const std::string &getRawRequestData() const { return rawRequestData; }

        bool getHeadersParsed() const { return headersParsed; }
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
    // request data
    HttpRequest request;

    std::string buffer;
    bool headersParsed;
    size_t contentLength;
    size_t expectedSize;
};

#endif
