#include "HttpResponse.hpp"

std::string HttpResponse::getReasonPhrase(unsigned short code) {
    // Use a static array of pairs for C++98 compatibility.
    struct StatusPhrase {
        unsigned short code;
        const char* phrase;
    };
    static const StatusPhrase statusPhrases[] = {
        {100, "Continue"},
        {101, "Switching Protocols"},
        {102, "Processing"},
        {103, "Early Hints"},
        {200, "OK"},
        {201, "Created"},
        {202, "Accepted"},
        {203, "Non-Authoritative Information"},
        {204, "No Content"},
        {205, "Reset Content"},
        {206, "Partial Content"},
        {207, "Multi-Status"},
        {208, "Already Reported"},
        {226, "IM Used"},
        {300, "Multiple Choices"},
        {301, "Moved Permanently"},
        {302, "Found"},
        {303, "See Other"},
        {304, "Not Modified"},
        {305, "Use Proxy"},
        {306, "Switch Proxy"},
        {307, "Temporary Redirect"},
        {308, "Permanent Redirect"},
        {400, "Bad Request"},
        {401, "Unauthorized"},
        {402, "Payment Required"},
        {403, "Forbidden"},
        {404, "Not Found"},
        {405, "Method Not Allowed"},
        {406, "Not Acceptable"},
        {407, "Proxy Authentication Required"},
        {408, "Request Timeout"},
        {409, "Conflict"},
        {410, "Gone"},
        {411, "Length Required"},
        {412, "Precondition Failed"},
        {413, "Payload Too Large"},
        {414, "URI Too Long"},
        {415, "Unsupported Media Type"},
        {416, "Range Not Satisfiable"},
        {417, "Expectation Failed"},
        {418, "I'm a teapot"},
        {421, "Misdirected Request"},
        {422, "Unprocessable Entity"},
        {423, "Locked"},
        {424, "Failed Dependency"},
        {425, "Too Early"},
        {426, "Upgrade Required"},
        {428, "Precondition Required"},
        {429, "Too Many Requests"},
        {431, "Request Header Fields Too Large"},
        {451, "Unavailable For Legal Reasons"},
        {500, "Internal Server Error"},
        {501, "Not Implemented"},
        {502, "Bad Gateway"},
        {503, "Service Unavailable"},
        {504, "Gateway Timeout"},
        {505, "HTTP Version Not Supported"},
        {506, "Variant Also Negotiates"},
        {507, "Insufficient Storage"},
        {508, "Loop Detected"},
        {510, "Not Extended"},
        {511, "Network Authentication Required"},
        // Custom codes
        {512, "Beqa is nerd"},
        {513, "Beqa's loop detected"},
        {514, "Internal Beqa's failure"},
        {515, "Beqa's version not supported"},
        {516, "Not Beqa's implemented"},
        {517, "Beqa's service unavailable"},
        {518, "Beqa's gateway"},
        {519, "Too many Beqas"},
        {520, "Too few Beqas"},
        {521, "Beqa's network authentication required"}
    };
    const size_t numPhrases = sizeof(statusPhrases) / sizeof(StatusPhrase);
    for (size_t i = 0; i < numPhrases; ++i) {
        if (statusPhrases[i].code == code)
            return statusPhrases[i].phrase;
    }
    return "Unknown Status";
}

std::string const HttpResponse::getDefaultStatusPage() {
    std::string errorMessage = this->getReasonPhrase(this->statusCode);
    std::string code = intToString(this->statusCode);
    
    std::string body = "<html>"
           "<head>"
           "<title>" + code + " " + errorMessage + "</title>"
           "</head>"
           "<body>"
           "<h1>" + code + " " + errorMessage + "</h1>"
           "<p>" + errorMessage + "</p>"
           "<hr>"
           "<address>" + PROJECT_NAME + "</address>"
           "</body>"
           "</html>";

    return body;
}

void HttpResponse::respondStatusPage(unsigned short code) {
    std::string errorMessage = this->getReasonPhrase(code);
    
    // ConfigValue *errors = &(this->config->at("errors"));
    config_map errors = Config::getSafe(this->config, "errors", ConfigValue()).getMap();
    std::string errorPage = Config::getSafe(errors, intToString(code), "").getString();
    
    this->setStatusCode(code);

    if(!errorPage.empty() && fileExists(ROOT_DIR + errorPage))
        this->body = readFileContent(ROOT_DIR + errorPage);
    else
        this->body = this->getDefaultStatusPage();
    
    this->setHeader("Content-Type", "text/html");
    this->setHeader("Content-Length", intToString(this->body.size()));

    this->build();
    // this->respond();
}

