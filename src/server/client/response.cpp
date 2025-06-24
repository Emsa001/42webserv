#include "Webserv.hpp"

/**
 * handleResponse builds and returns a complete HttpResponse object for the given request.
 * It sets the "Connection" header based on the client's request ("close" or "keep-alive").
 *
 * The method parses the request, checks for a matching location, validates the HTTP method,
 * and attempts to serve the requested file. If any error occurs (e.g., not found, invalid method),
 * it responds with the appropriate status page.
 *
 * @param request Pointer to the HttpRequest object containing the client's request data.
 * @return HttpResponse object containing the complete response string to be sent to the client.
 *
 * @note 
 * - SocketHandler will have to close or keep the connection alive based on the "Connection" header. (Can be accessed using response.getHeader("Connection")).
 * - The client timestamp update should be handled in SocketHandler, not here.
 */
HttpResponse Server::handleResponse(HttpRequest *request) {
    HttpResponse response(request, this->config);
    std::string connectionHeader = "close";

    try {
        // TODO: Should we parse it here? (Is nice because in one try catch block and automatically handles error page)
		request->parse();

        // Set the Connection header in the response based on the request
        response.setHeader("Connection", request->getHeader("Connection") == "close" ? "close" : "keep-alive");

        // Find the configuration for the requested URL path
        const config_map *location = this->findLocation(request->getURL()->getPath());
        response.setLocationData(location);
        if (location == NULL) {
            throw HttpRequestException(404); // No matching location found
        }
        
        // Validate that the HTTP method is allowed for this location
        this->isValidMethod(request, *location);
        
        // Attempt to create file data for the requested resource
        FileData fileData = this->createFileData(location, request);
        response.setFileData(fileData);

        if (!fileData.exists) 
            throw HttpRequestException(404); // File not found
       
        response.setSettings(); // Apply location-specific settings
        response.buildBody(fileData, request); // Build the response body

        // TODO: Client timestamp update should be handled in SocketHandler
        // this->client_timestamps[client_sock] = time(NULL);

    } catch(const HttpRequestException &e) {
        // Respond with the appropriate error status page
        response.respondStatusPage(e.getStatusCode());
    }

    // Return HttpResponse object containing the complete response string
    return response;
}


/*

    isValidMethod checks if the HTTP method of the request is allowed for the given location (configurable by user in config.yml).
    If the method is not allowed, it throws an HttpRequestException with a 405 status code.

*/
void Server::isValidMethod(HttpRequest *request, const config_map &location){
    std::string methods = Config::getSafe(location, "methods", (std::string)DEFAULT_METHODS).getString();
    if (methods.find(request->getMethod()) == std::string::npos) {
        throw HttpRequestException(405);
    }
}