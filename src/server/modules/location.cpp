/*

    These methods are used to find the location configuration that matches the given path.
    The location configuration is a map that contains the following keys:
    
    Location matching priority:
    1. Exact path matches (exact: true)
    2. Extension-based matches (extension: ".ext") with path prefix matching
    3. Best path prefix matches (longest match wins)

    findByExtension: Handles extension-based location matching
    findLocation: Main location finder that uses extension matching and path-based matching

*/

#include "Webserv.hpp"

// Best match approach
const config_map* Server::findLocation(const std::string &path) {
    const config_array& locations = this->config.at("locations").getArray();
    std::cout << "Finding location for path: " << path << std::endl;
    StringVec pathSegments = split(trimChar(path, '/'), '/');
    const config_map* match = NULL;

    // Extract file extension from path if it exists
    std::string fileExtension = "";
    size_t dotPos = path.find_last_of('.');
    if (dotPos != std::string::npos && dotPos > path.find_last_of('/')) {
        fileExtension = path.substr(dotPos);
    }

    // Priority 1: Check for exact path matches first
    for (config_array::const_iterator it = locations.begin(); it != locations.end(); ++it) {
        const config_map* location = &it->getMap();
        const bool exact = Config::getSafe(*location, "exact", false);
        const std::string locationPath = location->at("path").getString();

        if (exact && locationPath == path) {
            return location;
        }
    }

    // Priority 2: Regular path-based matching (best prefix match)
    for (config_array::const_iterator it = locations.begin(); it != locations.end(); ++it) {
        const config_map* location = &it->getMap();
        const bool exact = Config::getSafe(*location, "exact", false);
        const std::string locationPath = location->at("path").getString();

        // Skip exact and extension-based locations (already handled above)
        if (!exact) {
            StringVec locationSegments = split(trimChar(locationPath, '/'), '/');

            if (pathSegments.size() >= locationSegments.size() &&
                std::equal(locationSegments.begin(), locationSegments.end(), pathSegments.begin())) {
                if (!match || locationSegments.size() > split(match->at("path").getString(), '/').size())
                    match = location;
            }
        }
    }

    if(match) {
        FileData fileData = this->createFileData(match, path);
        if(fileData.exists)
            return match;
    }
    
    // Priority 3: Check for extension-based matches
    const config_map* extensionMatch = this->findByExtension(path);
    if (extensionMatch != NULL) {
        return extensionMatch;
    }


    return match;
}


// Extension-based location finder
const config_map* Server::findByExtension(const std::string &path) {
    const config_array& locations = this->config.at("locations").getArray();
    
    // Extract file extension from path if it exists
    std::string fileExtension = "";
    size_t dotPos = path.find_last_of('.');
    if (dotPos != std::string::npos && dotPos > path.find_last_of('/')) {
        fileExtension = path.substr(dotPos);
    }
    
    // If no extension found, return NULL
    if (fileExtension.empty())
        return NULL;
    
    StringVec pathSegments = split(trimChar(path, '/'), '/');

    for (config_array::const_iterator it = locations.begin(); it != locations.end(); ++it) {
        const config_map* location = &it->getMap();
        const std::string locationExtension = Config::getSafe(*location, "extension", std::string("")).getString();

        // Check for extension-based match
        if (!locationExtension.empty() && locationExtension == fileExtension) {
            // For extension-based locations, check if the path prefix matches
            const std::string locationPath = location->at("path").getString();
            StringVec locationSegments = split(trimChar(locationPath, '/'), '/');
            
            // If location path is "/" (root), it matches any path with the extension
            if (locationPath == "/" || locationSegments.empty()) {
                return location;
            }
            
            // Otherwise, check if the path matches or is a prefix
            if (pathSegments.size() >= locationSegments.size() &&
                std::equal(locationSegments.begin(), locationSegments.end(), pathSegments.begin())) {
                return location;
            }
        }
    }

    return NULL;
}



// First match approach

// const config_map* Server::findLocation(const std::string &path) {
//     const config_array& locations = this->config.at("locations").getArray();
//     StringVec pathSegments = split(removeTrailingSlash(path), '/');

//     for (config_array::const_iterator it = locations.begin(); it != locations.end(); ++it) {
//         const config_map* location = &it->getMap();
//         config_map::const_iterator exactIt = location->find("exact");
//         bool exact = (exactIt != location->end()) ? exactIt->second.getBool() : false;
//         const std::string& locationPath = location->at("path").getString();

//         if (exact && locationPath == path) {
//             return location;
//         }

//         StringVec locationSegments = split(removeTrailingSlash(locationPath), '/');
//         if (pathSegments.size() >= locationSegments.size() &&
//             std::equal(locationSegments.begin(), locationSegments.end(), pathSegments.begin())) {
//             return location;
//         }
//     }

//     return NULL;
// }
