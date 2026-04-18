#ifndef HTTP_MESSAGE_HPP
#define HTTP_MESSAGE_HPP

#include "Webserv.hpp"



class HttpMessage {
    protected:
        StringMultiMap headers;       ///< HTTP headers (key-value pairs)
        std::string body;        ///< HTTP message body (payload)

    public:
        HttpMessage() {}
        virtual ~HttpMessage() {}

        // --- Headers ---
        void setHeader(const std::string &key, const std::string &value) { 
            std::string mkey = toLower(key);
            headers.insert(std::make_pair(mkey, value)); 
        }
        const StringMultiMap &getHeaders() const { return headers; }
        std::string getHeader(const std::string &key) const {
            std::string mkey = toLower(key);
            StringMultiMap::const_iterator it = headers.find(mkey);
            return (it != headers.end()) ? it->second : "";
        }

        // --- Body ---
        void setBody(const std::string &b) { body = b; }
        const std::string &getBody() const { return body; }
};

#endif
