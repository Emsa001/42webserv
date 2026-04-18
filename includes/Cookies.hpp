#ifndef COOKIES_HPP
#define COOKIES_HPP

#include "Webserv.hpp"

class Cookie;
typedef std::vector<Cookie> CookieVector;
typedef std::map<std::string, CookieVector> DomainCookiesMap;

class Cookie {
    private:
        std::string key;
        std::string value;
        StringMap attributes;

    public:

        Cookie(){};
        Cookie(const std::string &key, const std::string &value) : key(key), value(value) {}
        ~Cookie(){};

        void setAttribute(const std::string &key, const std::string &value) {
            this->attributes[key] = value;
        }

        const std::string getAttribute(const std::string &key) const {
            StringMap::const_iterator it = this->attributes.find(key);
            if (it != this->attributes.end()) {
                return it->second;
            }
            return "";
        }

        const std::string getKey() const {
            return this->key;
        }

        const std::string getValue() const {
            return this->value;
        }

        const StringMap* getAttributes() const {
            return &this->attributes;
        }
};

class Cookies {
    private:
        DomainCookiesMap domainCookies;

    public:
        Cookies() {}
        ~Cookies() {}

        void addCookie(const std::string &domain, const Cookie &cookie) {
            domainCookies[domain].push_back(cookie);
        }

        const DomainCookiesMap* getCookies() const { return &domainCookies; }
        const std::vector<Cookie>* getCookies(const std::string &domain) const {
            DomainCookiesMap::const_iterator it = domainCookies.find(domain);
            if (it != domainCookies.end()) {
                return &(it->second);
            }
            return NULL;
        }

        void clear() { domainCookies.clear(); }
        void clear(const std::string &domain) { domainCookies.erase(domain); }

        void print() const {
           
        }
};


#endif
