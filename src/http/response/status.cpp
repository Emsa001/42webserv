#include "HttpResponse.hpp"

std::string HttpResponse::getReasonPhrase(unsigned short code) {
    switch (code) {
        case 100: return "Continue";
        case 101: return "Switching Protocols";
        case 102: return "Processing"; // WebDAV
        case 103: return "Early Hints"; 

        case 200: return "OK";
        case 201: return "Created";
        case 202: return "Accepted";
        case 203: return "Non-Authoritative Information";
        case 204: return "No Content";
        case 205: return "Reset Content";
        case 206: return "Partial Content";
        case 207: return "Multi-Status"; // WebDAV
        case 208: return "Already Reported"; // WebDAV
        case 226: return "IM Used"; // HTTP Delta encoding

        case 300: return "Multiple Choices";
        case 301: return "Moved Permanently";
        case 302: return "Found";
        case 303: return "See Other";
        case 304: return "Not Modified";
        case 305: return "Use Proxy";
        case 306: return "Switch Proxy"; // Deprecated
        case 307: return "Temporary Redirect";
        case 308: return "Permanent Redirect";

        case 400: return "Bad Request";
        case 401: return "Unauthorized";
        case 402: return "Payment Required";
        case 403: return "Forbidden";
        case 404: return "Not Found";
        case 405: return "Method Not Allowed";
        case 406: return "Not Acceptable";
        case 407: return "Proxy Authentication Required";
        case 408: return "Request Timeout";
        case 409: return "Conflict";
        case 410: return "Gone";
        case 411: return "Length Required";
        case 412: return "Precondition Failed";
        case 413: return "Payload Too Large";
        case 414: return "URI Too Long";
        case 415: return "Unsupported Media Type";
        case 416: return "Range Not Satisfiable";
        case 417: return "Expectation Failed";
        case 418: return "I'm a teapot"; // April Fools' joke but part of the spec
        case 421: return "Misdirected Request";
        case 422: return "Unprocessable Entity"; // WebDAV
        case 423: return "Locked"; // WebDAV
        case 424: return "Failed Dependency"; // WebDAV
        case 425: return "Too Early";
        case 426: return "Upgrade Required";
        case 428: return "Precondition Required";
        case 429: return "Too Many Requests";
        case 431: return "Request Header Fields Too Large";
        case 451: return "Unavailable For Legal Reasons";

        case 500: return "Internal Server Error";
        case 501: return "Not Implemented";
        case 502: return "Bad Gateway";
        case 503: return "Service Unavailable";
        case 504: return "Gateway Timeout";
        case 505: return "HTTP Version Not Supported";
        case 506: return "Variant Also Negotiates";
        case 507: return "Insufficient Storage"; // WebDAV
        case 508: return "Loop Detected"; // WebDAV
        case 510: return "Not Extended";
        case 511: return "Network Authentication Required";
        
        case 512: return "Beqa is nerd";
        case 513: return "Beqa's loop detected";
        case 514: return "Internal Beqa's failure";
        case 515: return "Beqa's version not supported";
        case 517: return "Beqa's service unavailable";
        case 516: return "Not Beqa's implemented";
        case 518: return "Beqa's gateway";
        case 519: return "Too many Beqas";
        case 520: return "Too few Beqas";
        case 521: return "Beqa's network authentication required";

        default: return "Unknown Status";
    }
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

