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
HttpResponse Server::handleResponse(HttpRequest *request)
{
    // Logger::debug(request->getRawRequestData());

    HttpResponse response(request, this->config);
    std::string connectionHeader = "close";
    
    try
    {
        request->parseHeaders(this->config);
        // Set the "Connection" header based on the request
        response.setHeader("Connection", request->getHeader("Connection") == "keep-alive" ? "keep-alive" : "close");
        
        // Find the location
        const config_map *location = this->findLocation(request->getURL()->getPath());
        if (location == NULL)
            throw HttpRequestException(404); // No matching location found
        
        request->parse(this->config, *location);
        // Check if redirection
        if (this->isRedirect(response, *location))
            return response;

        // Attempt to create file data for the requested resource
        FileData fileData = this->createFileData(location, request->getURL()->getPath());
        response.setFileData(fileData);
    
        // Check if the file exists
        if (!fileData.exists)
            throw HttpRequestException(404);

        // Validate the method
        if (!this->isValidMethod(request, *location))
            throw HttpRequestException(405);


        if (!request->getHeader("content-length").empty() &&
            request->getBodyLength() < stringToInt(request->getHeader("content-length")))
            return HttpResponse(); // invalid response, signals that we need to wait for body

        response.setSettings(*location);
        response.buildBody(fileData, request);
    }
    catch (const HttpRequestException &e)
    {
        response.respondStatusPage(e.getStatusCode());
    }

    return response;
}

bool Server::isValidMethod(HttpRequest *request, const config_map &location)
{
    std::string methods = Config::getSafe(location, "methods", (std::string)ALLOWED_METHODS).getString();
    if (std::string(ALLOWED_METHODS).find(request->getMethod()) == std::string::npos)
        return false;
    return methods.find(request->getMethod()) != std::string::npos;
}

bool Server::isRedirect(HttpResponse &response, const config_map &location)
{
    std::string redirect = Config::getSafe(location, "redirect", (std::string) "").getString();
    if (!redirect.empty())
    {
        response.setStatusCode(301); // Moved Permanently
        response.setHeader("Location", redirect);
        response.setBody(""); // No body for redirects
        response.build();
        return true;
    }

    return false;
}