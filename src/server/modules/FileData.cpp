#include "Webserv.hpp"

const FileData Server::createFileData(const config_map *location, std::string path) const{
    const std::string root = Config::getSafe(*location, "root", (std::string)ROOT_DIR).getString();
    std::string locationPath = trimChar(location->at("path").getString(), '/');

    if(path.empty()) path = "/";
    if(locationPath.empty()) locationPath = "/";
    
    std::string fullPath = std::string(ROOT_DIR) + root;
    fullPath += path.substr(locationPath.size());
    fullPath = trimChar(fullPath, '/');


    // std::cout << "Full path: " << fullPath << std::endl;
    // std::cout << "Request path: " << path << std::endl;
    // std::cout << "Location path: " << locationPath << std::endl;

    if(fullPath[fullPath.size() - 1] != '/' && locationPath == path){
        std::string index = Config::getSafe(*location, "index", (std::string)"index.html").getString();
        std::cout << "Adding index: " << index << std::endl;
        fullPath += "/" + index;
    }

    if(!Config::getSafe(*location, "extension", std::string("")).getString().empty()){
        std::string index = Config::getSafe(*location, "index", (std::string)"index.html").getString();
        fullPath = std::string(ROOT_DIR) + root + "/" + index;
    }

    // std::cout << std::endl;
    // std::cout << "Full path: " << fullPath << std::endl;
    // std::cout << "Request path: " << path << std::endl;
    // std::cout << "Location path: " << locationPath << std::endl;
    // std::cout << std::endl;

    return getFileData(fullPath);
}


