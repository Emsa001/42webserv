#include "Webserv.hpp"

bool ConfigSchema::validate(const std::string &key, ValueType type, int blockKind) const {
    const ConfigSchema *validSchema = this;

    if(ConfigParser::isReserved(key)) return true;

    switch(blockKind){
        case SERVER:
            if (nestedSchemas.find("server") != nestedSchemas.end()) {
                validSchema = &(nestedSchemas.find("server")->second);
            }
            break;
        case LOCATION:
            if (nestedSchemas.find("location") != nestedSchemas.end()) {
                validSchema = &(nestedSchemas.find("location")->second);
            }
            break;
        case ERRORS:
            if (nestedSchemas.find("errors") != nestedSchemas.end()) {
                validSchema = &(nestedSchemas.find("errors")->second);
            }
            break;
    }

    /*
        Check if allowAll is set and matches the key/value types
    */
    ValueType keyType = ConfigValue::detectType(key, false).getType();
    if(validSchema->allowAllKey != static_cast<ValueType>(-1) && 
       validSchema->allowAllValue != static_cast<ValueType>(-1) &&
       validSchema->allowAllKey == keyType && 
       validSchema->allowAllValue == type){
        return true;
    }

    if (validSchema->schema.find(key) == validSchema->schema.end()) {
        return false;
    }
    if (validSchema->schema.find(key)->second.type != type) {
        return false;
    }

    return true;
}

bool ConfigSchema::validateMap(config_map &map) const {
    int blockKind = map.find("blockKind")->second.getInt();

    if(map.find("locations") != map.end()){
        config_array locations = map.find("locations")->second.getArray();
        for(size_t i = 0; i < locations.size(); i++){
            config_map location = locations[i].getMap();
            if(this->validateMap(location) == false){
                return false;
            }
        }
    }

    if(map.find("errors") != map.end()){
        config_map errors = map.find("errors")->second.getMap();
        if(this->validateMap(errors) == false){
            return false;
        }
    }

    std::string schemaName;
    switch(blockKind){
        case SERVER:
            schemaName = "server";
            break;
        case LOCATION:
            schemaName = "location";
            break;
        case ERRORS:
            schemaName = "errors";
            break;
        default:
            schemaName = "root";
            break;
    }

    SchemaMap::const_iterator it;
    for (it = nestedSchemas.find(schemaName)->second.schema.begin(); it != nestedSchemas.find(schemaName)->second.schema.end(); ++it) {
        if(map.find(it->first) != map.end()) continue;

        
        if (it->second.required) {
            std::cerr << "Error: Required key '" << it->first << "' not found" << std::endl;
            return false;
        }
    }      

    return true;
}

bool ConfigSchema::validateRequired(const ConfigParser *config) const {

    config_map root = config->getRoot();
    config_array servers = config->getServers();

    SchemaMap::const_iterator it;
    for (it = schema.begin(); it != schema.end(); ++it) {
        if(root.find(it->first) != root.end()) continue;

        if (it->second.required) {
            std::cerr << "Error: Required key '" << it->first << "' not found" << std::endl;
            return false;
        }
    }

    for(size_t i = 0; i < servers.size(); i++){
        config_map server = servers[i].getMap();

        if(this->validateMap(server) == false){
            return false;
        }
    }

    return true;
}
