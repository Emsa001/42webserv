#ifndef HTTP_REQUEST_HPP
#define HTTP_REQUEST_HPP

#include "Webserv.hpp"

/*

https://www.rfc-editor.org/rfc/rfc2616#section-3.2.2

3.2.2 http URL

   The "http" scheme is used to locate network resources via the HTTP
   protocol. This section defines the scheme-specific syntax and
   semantics for http URLs.

   http_URL = "http:" "//" host [ ":" port ] [ abs_path [ "?" query ]]

   If the port is empty or not given, port 80 is assumed. The semantics
   are that the identified resource is located at the server listening
   for TCP connections on that port of that host, and the Request-URI
   for the resource is abs_path (section 5.1.2). The use of IP addresses
   in URLs SHOULD be avoided whenever possible (see RFC 1900 [24]). If
   the abs_path is not present in the URL, it MUST be given as "/" when
   used as a Request-URI for a resource (section 5.1.2). If a proxy
   receives a host name which is not a fully qualified domain name, it
   MAY add its domain to the host name it received. If a proxy receives
   a fully qualified domain name, the proxy MUST NOT change the host
   name.

*/

class HttpURL {

    private:      
        std::string path;
        std::string query;
        StringMap queryMap;

    public:
        HttpURL(const std::string &url);
        ~HttpURL(){}

        // Getters
        const std::string &getPath() const { return this->path; }
        const std::string &getQuery() const { return this->query; }
        const StringMap &getQueryMap() const { return this->queryMap; }
};

struct ClientRequestState {
    std::string buffer;
    bool headersParsed;
    size_t contentLength;
    size_t expectedSize;
};

class HttpRequest {
    private:
        StringMap headers;

        HttpURL *url;
        std::string method;
        std::string uri;
        std::string version;
        std::string body;

        size_t maxHeaderSize;
        size_t maxBodySize;

        std::string rawRequestData;

    public:
        HttpRequest(const std::string &rawData, const config_map &config) : url(NULL), rawRequestData(rawData) { 
            this->setMaxHeaderSize(Config::getSafe(config, "max_client_header_size", DEFAULT_MAX_HEADER_SIZE).getInt());
            this->setMaxBodySize(Config::getSafe(config, "max_client_body_size", DEFAULT_MAX_BODY_SIZE).getInt());
        }

        ~HttpRequest() {
            delete url;
        }

        void parse();

        // Setters
        void setHeader(const std::string &key, const std::string &value) { headers[key] = value; }
        void setMethod(const std::string &method) { this->method = method; }
        void setUri(const std::string &uri) { this->uri = uri; }
        void setVersion(const std::string &version) { this->version = version; }
        void setMaxHeaderSize(int size) { this->maxHeaderSize = size; }
        void setMaxBodySize(int size) { this->maxBodySize = size; }

        // Getters
        const std::string &getMethod() const { return this->method; }
        const std::string &getURI() const { return this->uri; }
        const std::string &getVersion() const { return this->version; }
        HttpURL *getURL() const { return this->url; }
        const StringMap &getHeaders() const { return this->headers; }
        const std::string &getBody() const { return this->body; }

        const std::string getHeader(const std::string &key) const {
            StringMap::const_iterator it = this->headers.find(key);
            if (it != this->headers.end()) {
                return it->second;
            }
            return "";
        }

        size_t getMaxHeaderSize() const { return this->maxHeaderSize; }
        size_t getMaxBodySize() const { return this->maxBodySize; }

};


class HttpRequestException : public std::exception {
    private:
        int statusCode;

    public:
        explicit HttpRequestException(int statusCode) : statusCode(statusCode) {}
        int getStatusCode() const { return statusCode; }
};


#endif
