#include "HttpResponse.hpp"

void HttpResponse::setSettings(const config_map &location) {
    if(Config::getSafe(location, "autoindex", false))
        this->listing = true;
    if(Config::getSafe(location, "cgi", false))
        this->cgi = true;
}
