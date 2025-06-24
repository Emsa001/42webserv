#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "Webserv.hpp"

#define LOG_ENABLED true

class Logger {
    private:
        static void log(const std::string& level, const std::string& color, const std::string& message) {
            if (!LOG_ENABLED) return;
            std::cout << BOLD << color << "[" << level << "] " << RESET << message << std::endl;
        }

    public:
        static void info(const std::string& message) {
            log("INFO", BLUE500, message);
        }

        static void success(const std::string& message) {
            log("SUCCESS", GREEN500, message);
        }

        static void warning(const std::string& message) {
            log("WARNING", YELLOW500, message);
        }

        static void error(const std::string& message) {
            log("ERROR", RED500, message);
        }

        static void critical(const std::string& message) {
            log("CRITICAL", RED900, message);
        }

        static void debug(const std::string& message) {
            log("DEBUG", GREEN500, message);
        }

        static void request(const std::string& message) {
            log("REQUEST", PURPLE400, message);
        }

        static void clientConnect(const int clientId) {
            log("CONNECT", GREEN500, "Client " + intToString(clientId) + " has connected.");
        }

        static void clientDisconnect(const int clientId) {
            log("DISCONNECT", ORANGE500, "Client " + intToString(clientId) + " has disconnected.");
        }

        static void clientIdle(const int clientId) {
            log("IDLE", ORANGE300, "Client " + intToString(clientId) + " is idle.");
        }
};

#endif

