#ifndef HTTP_RESPONSE_HPP
#define HTTP_RESPONSE_HPP

#include "Webserv.hpp"
#include "HttpRequest.hpp"
#include "HttpMessage.hpp"

/**
 * @brief Represents an HTTP response to be sent to the client.
 */
class HttpResponse : public HttpMessage {
    private:
        HttpRequest *request;            ///< Pointer to the associated request object
        config_map config;               ///< Server or location configuration

        FileData fileData;               ///< File data that was used to build the response (saved for debugging and testing)
        const config_map *locationData;   ///< Location-specific configuration data (saved for debugging and testing)
        
        std::string statusLine;          ///< Stores "HTTP/1.1 200 OK"
        std::string response;            ///< Entire response to be sent to the client
        
        bool listing;                    ///< True if body is a directory listing
        bool cgi;                        ///< True if body is CGI script output
        unsigned short statusCode;       ///< HTTP status code (e.g., 200)
        

        // --- Private helper methods ---

        /**
         * @brief Returns the default status page HTML for error responses.
         * @return Default status page HTML as a string
         */
        std::string const getDefaultStatusPage();

        std::string getReasonPhrase(unsigned short code);
        static std::string getMimeType(const std::string &path);

    public:
        // --- Constructor & Destructor ---

        /**
         * @brief Constructs an HttpResponse object.
         * @param request Pointer to the associated HttpRequest
         * @param config Reference to the configuration map
         */
        HttpResponse(HttpRequest *request, config_map &config)
            : request(request), config(config), statusLine("HTTP/1.1 200 OK"),
              listing(false), cgi(false), statusCode(200) {}

        ~HttpResponse() {}

        // --- Core response methods ---

        /**
         * @brief Builds full HTTP response string (ready to send).
         */
        void build();

        /**
         * @brief Logs the response (for debugging or access logs).
         */
        void log();

        /**
         * @brief Generates a directory listing as the response body.
         * @param fileData Information about the directory to list
         */
        void directoryListing(const FileData &fileData);

        /**
         * @brief Sets the response to a status page for the given code. 
         * @param code HTTP status code
         */
        void respondStatusPage(unsigned short code);
        void buildBody(FileData &fileData, const HttpRequest *request);

        // --- Setters ---

        void setResponse(const std::string &response) { this->response = response; }
        void setSettings();
        void setStatusCode(unsigned short code) {
            this->statusCode = code;
            statusLine = "HTTP/1.1 " + intToString(code) + " " + this->getReasonPhrase(code);
        }
        void setFileData(FileData &fileData) { this->fileData = fileData;}
        void setLocationData(const config_map *locationData) { this->locationData = locationData; }
        
        // --- Getters ---

        HttpRequest *getRequest() const { return request; }
        
        std::string getResponse() const { return response; }
        std::string getStatusLine() const { return statusLine; }
    
        unsigned short getStatusCode() const { return statusCode; }
        bool isCgi() const { return cgi; }
        bool isListing() const { return listing; }
        
        const FileData getFileData() const { return fileData; }
        const config_map *getLocationData() const { return locationData; }
};

#endif