#include "Webserv.hpp"

const FileData Server::createFileData(const config_map *location, HttpRequest *request) const{
    const std::string &root = location->at("root");
    std::string locationPath = trimChar(location->at("path").getString(), '/');
    std::string requestPath = request->getURL()->getPath();

    if(requestPath.empty()) requestPath = "/";
    if(locationPath.empty()) locationPath = "/";
    
    std::string fullPath = std::string(ROOT_DIR) + root;
    fullPath += requestPath.substr(locationPath.size());
    fullPath = trimChar(fullPath, '/');

    // std::cout << "Full path: " << fullPath << std::endl;
    // std::cout << "Request path: " << requestPath << std::endl;
    // std::cout << "Location path: " << locationPath << std::endl;

    if(fullPath[fullPath.size() - 1] != '/' && locationPath == requestPath){
        std::string index = Config::getSafe(*location, "index", (std::string)"index.html").getString();
        fullPath += "/" + index;
    }

    if(!Config::getSafe(*location, "extension", std::string("")).getString().empty()){
        std::string index = Config::getSafe(*location, "index", (std::string)"index.html").getString();
        // remove last element from fullPath so ./www//tester/test.bla -> ./www//tester
        if(fullPath[fullPath.size() - 1] == '/')
            fullPath += index;
        else if(fullPath.find_last_of('/') != std::string::npos)
            fullPath = fullPath.substr(0, fullPath.find_last_of('/')) + "/" + index;
        
        // add index to fullPath if it doesn't have an extension
        if(fullPath.find_last_of('.') == std::string::npos){
            fullPath += index;
        }
    }

    // std::cout << std::endl;
    // std::cout << "Full path: " << fullPath << std::endl;
    // std::cout << "Request path: " << requestPath << std::endl;
    // std::cout << "Location path: " << locationPath << std::endl;
    // std::cout << std::endl;

    return getFileData(fullPath);
}


