#include "Webserv.hpp"

void ConfigSchema::addEntry(const std::string &key, ValueType type, bool required)
{
    SchemaEntry entry;
    entry.type = type;
    entry.required = required;

    entry.max = -1; // Default value for max
    entry.min = -1; // Default value for min

    schema[key] = entry;
}

void ConfigSchema::addEntry(const std::string &key, NUMBER type, bool required)
{
    SchemaEntry entry;
    entry.type = INT;
    entry.required = required;

    entry.max = type.max;
    entry.min = type.min;

    schema[key] = entry;
}

void ConfigSchema::addNestedSchema(const std::string &key, const ConfigSchema &nestedSchema)
{
    nestedSchemas[key] = nestedSchema;
}

void ConfigSchema::allowAll(ValueType key, ValueType value)
{
    this->allowAllKey = key;
    this->allowAllValue = value;
}

ConfigSchema createSchema()
{
    ConfigSchema rootSchema;
    rootSchema.addEntry("log_format", STRING, false);

    // Server Schema
    ConfigSchema serverSchema;
    serverSchema.addEntry("server_name", STRING, true);
    serverSchema.addEntry("host", STRING, false);
    serverSchema.addEntry("listen", STRING, true);
    serverSchema.addEntry("max_client_body_size", NUMBER(1, INT32_MAX), true);
    serverSchema.addEntry("max_client_header_size", NUMBER(1, INT32_MAX), true);
    serverSchema.addEntry("keep_alive", NUMBER(1, 1000), false);

    rootSchema.addNestedSchema("server", serverSchema);

    // Location Schema
    ConfigSchema locationSchema;
    locationSchema.addEntry("path", STRING, true);
    locationSchema.addEntry("exact", BOOL, false);
    locationSchema.addEntry("methods", STRING, false);
    locationSchema.addEntry("root", STRING, false);
    locationSchema.addEntry("index", STRING, false);
    locationSchema.addEntry("redirect", STRING, false);
    locationSchema.addEntry("autoindex", BOOL, false);
    locationSchema.addEntry("upload_path", STRING, false);
    locationSchema.addEntry("extension", STRING, false);

    locationSchema.addEntry("cgi", BOOL, false);

    rootSchema.addNestedSchema("location", locationSchema);

    // Error Schema
    ConfigSchema errorSchema;
    errorSchema.allowAll(INT, STRING);

    rootSchema.addNestedSchema("errors", errorSchema);

    return rootSchema;
}

/*

Execute CGI based on certain file extension ?
Because you won’t call the CGI directly, use the full path as ?


*/
