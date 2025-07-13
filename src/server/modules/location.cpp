/*

    This method is used to find the location configuration that matches the given path.
    The location configuration is a map that contains the following keys:
    
    Location matching priority:
    1. Exact path matches (exact: true)
    2. Extension-based matches (extension: ".ext") with path prefix matching
    3. Best path prefix matches (longest match wins)

*/

#include "Webserv.hpp"

// Best match approach

const config_map* Server::findLocation(const std::string &path) {

    const config_array& locations = this->config.at("locations").getArray();
    StringVec pathSegments = split(trimChar(path, '/'), '/');
    const config_map* match = NULL;

    // Extract file extension from path if it exists
    std::string fileExtension = "";
    size_t dotPos = path.find_last_of('.');
    if (dotPos != std::string::npos && dotPos > path.find_last_of('/')) {
        fileExtension = path.substr(dotPos);
    }

    for (config_array::const_iterator it = locations.begin(); it != locations.end(); ++it) {
        const config_map* location = &it->getMap();
        const bool exact = Config::getSafe(*location, "exact", false);
        const std::string locationPath = location->at("path").getString();
        const std::string locationExtension = Config::getSafe(*location, "extension", std::string("")).getString();

        // Check for exact path match first
        if (exact && locationPath == path) {
            return location;
        }

        // Check for extension-based match
        if (!locationExtension.empty() && !fileExtension.empty() && locationExtension == fileExtension) {
            // Also check if the path matches or is a prefix
            StringVec locationSegments = split(trimChar(locationPath, '/'), '/');
            if (pathSegments.size() >= locationSegments.size() &&
                std::equal(locationSegments.begin(), locationSegments.end(), pathSegments.begin())) {
                return location; // Extension match has priority
            }
        }

        // Regular path-based matching (non-exact)
        if (!exact && locationExtension.empty()) {
            StringVec locationSegments = split(trimChar(locationPath, '/'), '/');

            if (pathSegments.size() >= locationSegments.size() &&
                std::equal(locationSegments.begin(), locationSegments.end(), pathSegments.begin())) {
                if (!match || locationSegments.size() > split(match->at("path").getString(), '/').size()) {
                    match = location;
                }
            }
        }
    }

    return match;
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
