/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: escura <escura@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/05 14:45:07 by escura            #+#    #+#             */
/*   Updated: 2025/06/22 22:00:10 by escura           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "Webserv.hpp"

class ConfigValue;
class ConfigSchema;
class ConfigParser;

enum ValueType { INT, BOOL, STRING, ARRAY, MAP, ANY };
enum blockKind { SERVER, LOCATION, ERRORS, UNKNOWN };

typedef std::map<std::string, ConfigValue> config_map;
typedef std::vector<ConfigValue> config_array;

class ConfigValue {
    private:
        ValueType type;

        int i;
        bool b;
        std::string s;
        config_array a;
        config_map m;

        void clear();

    public:
        ConfigValue();
        ConfigValue(int i);
        ConfigValue(bool b);
        ConfigValue(std::string &str);
        ConfigValue(const std::string &str);
        ConfigValue(const config_array &array);
        ConfigValue(const config_map &map);
        ConfigValue(const ConfigValue &other);
        ConfigValue& operator=(const ConfigValue &other);
        ~ConfigValue();

        ValueType getType() const { return type; }
        bool isEmpty() const {
            if (type == STRING) return s.empty();
            if (type == ARRAY) return a.empty();
            if (type == MAP) return m.empty();
            if (type == INT) return i == 0;
            return true;
        }

        int getInt() const { return (type == INT) ? i : 0; }
        bool getBool() const { return (type == BOOL) ? b : false; }
        const std::string &getString() const { return s; }
        const config_array &getArray() const { return a; }
        const config_map& getMap() const { return m; }
        config_map &getMap() { return m; }

        // Type conversions
        operator std::string() const;
        operator int() const;
        operator bool() const;
        operator config_array() const;
        operator config_map() const;

        bool operator==(const std::string &other) const;
        bool operator==(const int &other) const;
        bool operator==(const bool &other) const;

        void print() const;
        static ConfigValue detectType(const std::string &value, bool forceString = false);
        
};

std::ostream& operator<<(std::ostream& os, const ConfigValue& cv);

std::string type_to_string(ValueType type);

class ConfigSchema {
    public:
        struct SchemaEntry {
            ValueType type;
            bool required;
        };

        typedef std::map<std::string, SchemaEntry> SchemaMap;
        typedef std::map<std::string, ConfigSchema> NestedSchemaMap;

    private:
        ValueType allowAllKey;
        ValueType allowAllValue;

        SchemaMap schema;
        NestedSchemaMap nestedSchemas;

    public:
        ConfigSchema() : allowAllKey(static_cast<ValueType>(-1)), allowAllValue(static_cast<ValueType>(-1)) {};
        ~ConfigSchema() {};

        void addEntry(const std::string &key, ValueType type, bool required);
        
        void addNestedSchema(const std::string &key, const ConfigSchema &nestedSchema);
        bool validate(const std::string &key, ValueType type, int blockKind) const;
        bool validateRequired(const ConfigParser *config) const;
        bool validateMap(config_map &map) const;

        void allowAll(ValueType key, ValueType value) {
            this->allowAllKey = key;
            this->allowAllValue = value;
        }

        ValueType getAllowAllKey() const { return this->allowAllKey; }
        ValueType getAllowAllValue() const { return this->allowAllValue; }

        void print(int indent = 0) const {
            std::string indentStr(indent, ' ');

            // Iterate over schema entries
            SchemaMap::const_iterator it;
            for (it = schema.begin(); it != schema.end(); ++it) {
                std::cout << indentStr << "Key: " << it->first
                        << " (Type: " << type_to_string(it->second.type)
                        << ", Required: " << (it->second.required ? "true" : "false")
                        << ")" << std::endl;
            }

            NestedSchemaMap::const_iterator nestedIt;
            for (nestedIt = nestedSchemas.begin(); nestedIt != nestedSchemas.end(); ++nestedIt) {
                std::cout << indentStr << "Nested Schema: " << nestedIt->first << std::endl;
                nestedIt->second.print(indent + 4);
            }
        }
};

class ConfigParser
{
    private:        
        config_map root;
        ConfigSchema schema;

        // temp
        std::vector<config_map> blocks;
        config_map *block;
        int blockId;
        // temp

        std::ifstream file;
        std::string line;
        int ln;

        int indent;
        int expectedIndent;

        void init();

        bool processLine();
        bool handleComment(char p, char c, char quote, size_t i);
        bool handleIndentation(char c, char quote, const std::string &key, int previousIndent);
        bool handleQuotes(char c, char &quote);
        int handleKeyValueSeparator(char c, char n, bool *isValue, size_t *i, std::string &key);
        void createNewBlock(const std::string &key);
        bool validateAndSetKey(char quote, const std::string &key, const std::string &value, bool forceString);
        void setBlock();

        bool setKey(std::string const &key, std::string const &value, bool forceString);
        void updateParents();

        bool setKeyInBlock(const std::string &key, const ConfigValue &typedValue);
        config_map* findParentBlock(int blockId, int level);
        void updateParentBlock(config_map* parent, const std::string &blockName, int blockId);
        
        config_map cleanTemp(config_map *temp);
                
    public:
        ConfigParser();
        ConfigParser(std::string const &filename);
        ~ConfigParser();

        bool parse();
        static bool isReserved(std::string const &key);

        config_map getRoot() const { return root; }
        config_array getServers() const {
            config_array servers = config_array();

            config_map::const_iterator it = this->root.begin();
            while (it != this->root.end()) {
                if (it->second.getMap().count("listen")) {
                    servers.push_back(it->second);
                }
                it++;
            }

            return servers;
        };

    
    class ParseError : public std::exception {
        private:
            int ln;
            std::string msg;
            std::string full_msg;
        public:
            ParseError(int line, const std::string& message) : ln(line), msg(message) {
                full_msg = "Error at line: " + intToString(ln) + " - " + msg;
            }
            virtual ~ParseError() throw() {}
            const char* what() const throw() {
                return full_msg.c_str();
            }
    };
};

class Config{
    private:
        Config() { };
        config_map root;
        config_array servers;

    public:
        ~Config() {};

        static Config& instance(){
            static Config instance;
            return instance;
        }

        void parse(std::string const &filename){
            ConfigParser parser(filename);
            if (!parser.parse()) {
                throw std::runtime_error("Error parsing config file");
            }
            this->root = parser.getRoot();
            this->servers = parser.getServers();
        }

        const config_map &getRoot() const { return this->root; }
        const config_array &getServers() const { return this->servers; }

        // Will return the value of the key if it exists, otherwise it will return the default value
        static ConfigValue getSafe(const config_map &map, const std::string &key, const ConfigValue &defaultValue = ConfigValue()){
            config_map::const_iterator it = map.find(key);
            return (it != map.end()) ? it->second : defaultValue;
        }
};

// functions

ConfigSchema createSchema();

#endif
