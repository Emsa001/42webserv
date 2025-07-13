#include "Webserv.hpp"


// Function object for trimming from start
struct IsNotSpace {
    bool operator()(unsigned char ch) const {
        return !std::isspace(ch);
    }
};


void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), IsNotSpace()));
}

void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), IsNotSpace()).base(), s.end());
}


void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}

std::string intToString(int i) {
    std::stringstream ss;
    if(!ss.good())
        throw std::invalid_argument("Invalid integer");
    ss << i;
    return ss.str();
}

int stringToInt(const std::string &s) {
    std::stringstream ss(s);
    if(!ss.good())
        throw std::invalid_argument("Invalid integer string: " + s);
    int i;
    ss >> i;
    return i;
}

// Helper function to convert enum to string
std::string type_to_string(ValueType type) {
    switch (type) {
        case STRING: return "STRING";
        case INT: return "INT";
        case BOOL: return "BOOL";
        default: return "UNKNOWN";
    }
}

StringVec split(const std::string &s, char delim) {
    StringVec result;
    std::stringstream ss(s);
    std::string item;

    while (std::getline(ss, item, delim)) {
        result.push_back(item);
    }

    return result;
}

StringVec splitFirst(const std::string &s, char delim) {
    StringVec result;
    size_t pos = s.find(delim);
    
    if (pos == std::string::npos) {
        result.push_back(s);
    } else {
        result.push_back(s.substr(0, pos));
        result.push_back(s.substr(pos + 1));
    }

    return result;
}


std::string trimChar(const std::string &str, char c) {
    if (str.empty()) return str;
    std::string trimmed = str;
    while (!trimmed.empty() && trimmed[trimmed.size() - 1] == c) {
        trimmed.erase(trimmed.size() - 1);
    }
    return trimmed;
}

std::string toLower(const std::string &str) {
    std::string newstr = "";
    for (std::string::const_iterator it = str.begin(); it != str.end(); ++it) {
        newstr += (std::tolower(*it));
    }
    return newstr;
}