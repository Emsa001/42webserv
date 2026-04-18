#include "HttpResponse.hpp"

#define DEFAULT_LOG_FORMAT "[server] - [time] [[method]]: [uri] - [status] ([reason]) - [size]"

std::string getDate();
std::string getTime();

void HttpResponse::log() {

    Config config = Config::instance();
    std::string logFormat = Config::getSafe(config.getRoot(), "log_format", (std::string)DEFAULT_LOG_FORMAT).getString();
    std::string serverName = Config::getSafe(this->config, "server_name", "webserv").getString();

    std::string date = getDate();
    std::string time = getTime();
    std::string method = this->request->getMethod();
    std::string uri = this->request->getURI();
    std::string status = intToString(this->statusCode);
    std::string size = intToString(this->body.size());

    // Replace placeholders in the log format
    std::string logMessage = logFormat;
    size_t pos;
    while ((pos = logMessage.find("[server]")) != std::string::npos) {
        logMessage.replace(pos, 8, serverName);
    }
    while ((pos = logMessage.find("[date]")) != std::string::npos) {
        logMessage.replace(pos, 6, date);
    }
    while ((pos = logMessage.find("[time]")) != std::string::npos) {
        logMessage.replace(pos, 6, time);
    }
    while ((pos = logMessage.find("[method]")) != std::string::npos) {
        logMessage.replace(pos, 8, method);
    }
    while ((pos = logMessage.find("[uri]")) != std::string::npos) {
        logMessage.replace(pos, 5, uri);
    }
    while ((pos = logMessage.find("[status]")) != std::string::npos) {
        logMessage.replace(pos, 8, status);
    }
    while ((pos = logMessage.find("[reason]")) != std::string::npos) {
        logMessage.replace(pos, 8, this->getReasonPhrase(this->statusCode));
    }
    while ((pos = logMessage.find("[size]")) != std::string::npos) {
        logMessage.replace(pos, 6, size);
    }
    // Log the message
    Logger::request(logMessage);
}

std::string getDate() {
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    localtime_r(&now, &tstruct);
    strftime(buf, sizeof(buf), "%Y-%m-%d", &tstruct);
    return std::string(buf);
}

std::string getTime() {
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    localtime_r(&now, &tstruct);
    strftime(buf, sizeof(buf), "%H:%M:%S", &tstruct);
    return std::string(buf);
}
