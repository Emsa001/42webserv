/*

    This method is used to find the location configuration that matches the given path.
    The location configuration is a map that contains the following keys:

*/

#include "Webserv.hpp"

// Best match approach

const config_map* Server::findLocation(const std::string &path) {

    const config_array& locations = this->config.at("locations").getArray();
    StringVec pathSegments = split(trimChar(path, '/'), '/');
    const config_map* match = NULL;

    for (config_array::const_iterator it = locations.begin(); it != locations.end(); ++it) {
        const config_map* location = &it->getMap();
        const bool exact = Config::getSafe(*location, "exact", false);
        const std::string locationPath = location->at("path").getString();

        if (!exact) {
            StringVec locationSegments = split(trimChar(locationPath, '/'), '/');

            if (pathSegments.size() >= locationSegments.size() &&
                std::equal(locationSegments.begin(), locationSegments.end(), pathSegments.begin())) {
                if (!match || locationSegments.size() > split(match->at("path").getString(), '/').size()) {
                    match = location;
                }
            }

        } else if (locationPath == path){
            return location;
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
