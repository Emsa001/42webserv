#include "Webserv.hpp"

ConfigValue::ConfigValue() : type(INT), i(0)
{
}

ConfigValue::ConfigValue(bool b) : type(BOOL), b(b)
{
}

ConfigValue::ConfigValue(int i) : type(INT), i(i)
{
}

ConfigValue::ConfigValue(std::string const &str) : type(STRING)
{
    s = str;
}

ConfigValue::ConfigValue(config_array const &array) : type(ARRAY)
{
    a = array;
}

ConfigValue::ConfigValue(config_map const &map) : type(MAP)
{
    m = map;
}

ConfigValue::ConfigValue(ConfigValue const &other)
{
    type = other.type;
    switch (type)
    {
    case BOOL:
        b = other.b;
        break;
    case INT:
        i = other.i;
        break;
    case STRING:
        s = other.s;
        break;
    case ARRAY:
        a = other.a;
        break;
    case MAP:
        m = other.m;
        break;
    default:
        break;
    }
}

ConfigValue::~ConfigValue()
{
}

// Operators

ConfigValue &ConfigValue::operator=(const ConfigValue &other)
{
    if (this != &other)
    {
        type = other.type;
        switch (type)
        {
        case INT:
            i = other.i;
            break;
        case BOOL:
            b = other.b;
            break;
        case STRING:
            s = other.s;
            break;
        case ARRAY:
            a = other.a;
            break;
        case MAP:
            m = other.m;
            break;
        default:
            break;
        }
    }
    return *this;
}

ConfigValue::operator std::string() const
{
    return (type == STRING) ? s : "";
}

ConfigValue::operator int() const
{
    return (type == INT) ? i : 0;
}

ConfigValue::operator bool() const
{
    return (type == BOOL) ? b : false;
}

ConfigValue::operator config_array() const
{
    return (type == ARRAY) ? a : config_array();
}

ConfigValue::operator config_map() const
{
    return (type == MAP) ? m : config_map();
}

bool ConfigValue::operator==(const std::string &other) const
{
    return (type == STRING) && (s == other);
}

bool ConfigValue::operator==(const int &other) const
{
    return (type == INT) && (i == other);
}

bool ConfigValue::operator==(const bool &other) const
{
    return (type == BOOL) && (b == other);
}

std::ostream &operator<<(std::ostream &os, const ConfigValue &cv)
{
    switch (cv.getType())
    {
    case INT:
        os << cv.getInt();
        break;
    case BOOL:
        os << (cv.getBool() ? "true" : "false");
        break;
    case STRING:
        os << '"' << cv.getString() << '"';
        break;
    case ARRAY:
        os << "[ ";
        for (size_t i = 0; i < cv.getArray().size(); i++)
        {
            if (i > 0)
                os << ", ";
            os << (cv.getArray())[i];
        }
        os << " ]";
        break;
    case MAP:
        os << "{ ";
        for (config_map::const_iterator it = cv.getMap().begin(); it != cv.getMap().end(); ++it)
        {
            if (it != cv.getMap().begin())
                os << ", ";
            os << it->first << ": " << it->second;
        }
        os << " }";
        break;
    default:
        break;
    }
    return os;
}

// Member functions

ConfigValue ConfigValue::detectType(const std::string &value, bool forceString)
{
    if (forceString)
        return ConfigValue(value);

    if (value == "true")
        return ConfigValue(true);
    if (value == "false")
        return ConfigValue(false);

    bool isNumber = true;
    size_t start = 0;
    if (!value.empty() && value[0] == '-')
        start = 1;
    for (size_t i = start; i < value.size(); i++)
    {
        if (!isdigit(value[i]))
        {
            isNumber = false;
            break;
        }
    }
    if (start == 1 && value.size() == 1)
        isNumber = false;
    if (isNumber)
        return ConfigValue(stringToInt(value));

    return ConfigValue(value);
}
