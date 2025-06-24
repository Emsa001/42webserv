#ifndef HTTP_MESSAGE_HPP
#define HTTP_MESSAGE_HPP

#include "Webserv.hpp"



class HttpMessage {
    protected:
        StringMap headers;       ///< HTTP headers (key-value pairs)
        std::string body;        ///< HTTP message body (payload)

    public:
        HttpMessage() {}
        virtual ~HttpMessage() {}

        // --- Headers ---
        void setHeader(const std::string &key, const std::string &value) { headers[key] = value; }
        const StringMap &getHeaders() const { return headers; }
        std::string getHeader(const std::string &key) const {
            StringMap::const_iterator it = headers.find(key);
            return (it != headers.end()) ? it->second : "";
        }

        // --- Body ---
        void setBody(const std::string &b) { body = b; }
        const std::string &getBody() const { return body; }

        // Virtual for polymorphism, can be overridden by subclasses if needed
        virtual void log() const {}
};

#endif
