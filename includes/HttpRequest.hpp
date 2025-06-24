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

struct ClientRequestState {
    std::string buffer;
    bool headersParsed;
    size_t contentLength;
    size_t expectedSize;
};

class HttpRequest : public HttpMessage {
    private:
        HttpURL *url;
        std::string method;
        std::string uri;
        std::string version;

        size_t maxHeaderSize;
        size_t maxBodySize;

        std::string rawRequestData;

    public:
        HttpRequest(const std::string &rawData, const config_map &config)
            : url(NULL), rawRequestData(rawData) {
            maxHeaderSize = Config::getSafe(config, "max_client_header_size", DEFAULT_MAX_HEADER_SIZE).getInt();
            maxBodySize = Config::getSafe(config, "max_client_body_size", DEFAULT_MAX_BODY_SIZE).getInt();
        }

        ~HttpRequest() { delete url; }

        void parse();

        // --- Setters ---
        void setMethod(const std::string &m) { method = m; }
        void setUri(const std::string &u) { uri = u; }
        void setVersion(const std::string &v) { version = v; }
        void setMaxHeaderSize(size_t size) { maxHeaderSize = size; }
        void setMaxBodySize(size_t size) { maxBodySize = size; }

        // --- Getters ---
        const std::string &getMethod() const { return method; }
        const std::string &getURI() const { return uri; }
        const std::string &getVersion() const { return version; }
        HttpURL *getURL() const { return url; }
        const std::string &getRawRequestData() const { return rawRequestData; }
        size_t getMaxHeaderSize() const { return maxHeaderSize; }
        size_t getMaxBodySize() const { return maxBodySize; }
};

class HttpRequestException : public std::exception {
    private:
        int statusCode;

    public:
        explicit HttpRequestException(int statusCode) : statusCode(statusCode) {}
        int getStatusCode() const { return statusCode; }
};

#endif
