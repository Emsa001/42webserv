#include "HttpResponse.hpp"

void HttpResponse::setSettings(){
    if(Config::getSafe(*this->getLocationData(), "autoindex", false))
        this->listing = true;
    if(Config::getSafe(*this->getLocationData(), "cgi", false))
        this->cgi = true;
}
