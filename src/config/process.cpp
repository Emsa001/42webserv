#include "Webserv.hpp"

bool ConfigParser::processLine()
{
    std::string &line = this->line;
    std::string key, value;

    char quote = '\0';
    bool forceString = false;

    bool isValue = false;

    int previousIndent = this->indent;
    this->indent = 0;

    for (size_t i = 0; i < line.size(); i++)
    {
        char p = i > 0 ? line[i - 1] : '\0';
        char c = line[i];
        char n = line.size() > i + 1 ? line[i + 1] : '\0';
        
        if(!key.empty() && (std::isspace(c) && quote == '\0')) continue;

        if (handleComment(p, c, quote, i))
            break;

        if (handleIndentation(c, quote, key, previousIndent))
            continue;

        if (handleQuotes(c, quote)){
            forceString = true;
            continue;
        }

        int separator = handleKeyValueSeparator(c, n, &isValue, &i, key);
        if (separator == 1) return true;
        else if (separator == 0) continue;

        if (isValue) value += c;
        else key += c;
    }

    return validateAndSetKey(quote, key, value, forceString);
}

int ConfigParser::handleKeyValueSeparator(char c, char n, bool *isValue, size_t *i, std::string &key)
{
    if ((c == ':') && !(*isValue))
    {
        if (n == '\0' || n == '\n'){
            this->expectedIndent = this->indent + 4;
            return (createNewBlock(key), 1);
        }
        else if (std::isspace(n)){
            *isValue = true;
            *i += 1;
            return 0;
        }
    }

    return -1;
}

bool ConfigParser::handleComment(char p, char c, char quote, size_t i)
{
    if (c == '#' && (std::isspace(p) || i == 0) && quote == '\0')
        return true;
    return false;
}

bool ConfigParser::handleIndentation(char c, char quote, const std::string &key, int previousIndent)
{
    if (std::isspace(c) && quote == '\0' && key.empty())
    {
        this->indent++;
        return true;
    }

    if(this->expectedIndent != -1){
        if(this->expectedIndent != this->indent)
            throw ParseError(this->ln, "Indentation not matching");

        this->expectedIndent = -1;
    }

    if(this->indent > previousIndent + 4)
        throw ParseError(this->ln, "Indentation not matching");

    return false;
}

bool ConfigParser::handleQuotes(char c, char &quote)
{
    if (c == '"' || c == '\'')
    {
        if (quote == '\0')
            quote = c;
        else if (quote == c)
            quote = '\0';
        return true;
    }
    return false;
}

bool ConfigParser::validateAndSetKey(char quote, const std::string &key, const std::string &value, bool forceString)
{
    if(key.empty() && value.empty())
        return true;
    if (this->indent > 0 && this->block == NULL)
        throw ParseError(this->ln, "Indentation not matching");
    if (quote != '\0')
        throw ParseError(this->ln, "Quote not closed");
    if (key.empty() && (!this->block || this->block->at("blockType").getString() != "array"))
        throw ParseError(this->ln, "Key not found");
    if (value.empty())
        throw ParseError(this->ln, "Value not found");

    int blockKind = this->block ? this->block->at("blockKind").getInt() : -1;

    try{
        this->schema.validate(key, ConfigValue::detectType(value, forceString).getType(), value, blockKind);
    }catch (const std::runtime_error &e) {
        throw ParseError(this->ln, e.what());
    }

    return setKey(key, value, forceString);
}

bool ConfigParser::setKey(const std::string &key, const std::string &value, bool forceString) {
    this->setBlock();
    ConfigValue typedValue = ConfigValue::detectType(value, forceString);
    
    if (ConfigParser::isReserved(key)) {
        throw ParseError(this->ln, "'" + key + "' is a reserved keyword");
    }
    
    if (this->block != NULL)
        return setKeyInBlock(key, typedValue);
    
    this->root[key] = typedValue;

    return true;
}
