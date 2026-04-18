#include "HttpRequest.hpp"

HttpURL::HttpURL(const std::string &url) {
    StringVec parts = splitFirst(url, '?');

    if (parts.empty()) {
        std::cerr << "Invalid URL" << std::endl;
        this->path = "";
        this->query = "";
        return;
    }

    this->path = parts[0];
    this->query = (parts.size() > 1) ? parts[1] : "";

    if (this->query.empty()) {
        return;
    }

    StringVec queryParts = split(this->query, '&');
    for (StringVec::const_iterator it = queryParts.begin(); it != queryParts.end(); ++it) {
        StringVec pair = splitFirst(*it, '=');

        if (pair.empty() || pair[0].empty()) {
            continue;
        }

        const std::string &key = pair[0];
        const std::string &value = (pair.size() > 1) ? pair[1] : "";

        this->queryMap[key] = value;
    }
}
