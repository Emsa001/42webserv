#include "HttpRequest.hpp"

/**
 * @brief Parses the raw HTTP request data and populates the HttpRequest object.
 *
 * This method processes the raw HTTP request stored in `rawRequestData`, extracting the request line,
 * headers, and body. It performs validation on the request format and enforces size limits for headers and body.
 *
 * Steps performed:
 * 1. Locates the end of the HTTP headers (marked by "\r\n\r\n").
 * 2. Splits the request into header and body parts.
 * 3. Parses the request line to extract the HTTP method, URI, and version.
 * 4. Iterates through each header line, extracting key-value pairs and trimming whitespace.
 * 5. Accumulates the total header size and throws exceptions if size limits are exceeded.
 * 6. For POST and DELETE methods, assigns the body and checks its size against the maximum allowed.
 *
 * @throws HttpRequestException
 *         - 400: If the request is malformed (headers not properly terminated).
 *         - 414: If the request line exceeds the maximum allowed header size.
 *         - 431: If the total headers size exceeds the maximum allowed.
 *         - 413: If the body size exceeds the maximum allowed for POST or DELETE.
 *
 * @note
 * - The method assumes `rawRequestData` is already populated.
 * - The `maxHeaderSize` and `maxBodySize` members define the allowed limits for headers and body.
 * - The `headers` map is populated with parsed header fields.
 * - The `url` member is initialized with the parsed URI.
 */
void HttpRequest::parse(const config_map &serverConfig) {
    int maxHeaderSize = Config::getSafe(serverConfig, "max_client_header_size", DEFAULT_MAX_HEADER_SIZE).getInt();
    int maxBodySize = Config::getSafe(serverConfig, "max_client_body_size", DEFAULT_MAX_BODY_SIZE).getInt();

    // 1. Locate the end of the HTTP headers (marked by "\r\n\r\n")
    size_t headerEnd = rawRequestData.find("\r\n\r\n");
    if (headerEnd == std::string::npos)
        throw HttpRequestException(400);

    // 2. Split the request into header and body parts
    std::string headerPart = rawRequestData.substr(0, headerEnd);
    std::string bodyPart = rawRequestData.substr(headerEnd + 4);

    std::istringstream headerStream(headerPart);
    std::string line;

    // 3. Parse the request line to extract the HTTP method, URI, and version
    std::getline(headerStream, line);
    size_t totalHeaderSize = line.size();
    if (totalHeaderSize > maxHeaderSize) {
        throw HttpRequestException(414);
    }

    std::istringstream lineStream(line);
    lineStream >> this->method >> this->uri >> this->version;

    this->url = new HttpURL(this->normalizeUri(this->uri));

    // 4. Iterate through each header line, extracting key-value pairs and trimming whitespace
    while (std::getline(headerStream, line) && line != "\r") {
        if (line.empty() || line == "\r\n") break;

        size_t pos = line.find(": ");
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 2);
            key.erase(0, key.find_first_not_of(" \t\n\r"));
            key.erase(key.find_last_not_of(" \t\n\r") + 1);
            value.erase(0, value.find_first_not_of(" \t\n\r"));
            value.erase(value.find_last_not_of(" \t\n\r") + 1);

            // 5. Accumulate the total header size and throw exceptions if size limits are exceeded
            totalHeaderSize += line.size();
            this->setHeader(key, value);
        }

        if (totalHeaderSize > maxHeaderSize) {
            throw HttpRequestException(431);
        }
    }

    // 6. For POST and DELETE methods, assign the body and check its size against the maximum allowed
    if (this->method == "POST" || this->method == "DELETE") {
        this->body = bodyPart;
        if (this->body.size() > maxBodySize) {
            throw HttpRequestException(413);
        }
        if(this->body.size() > 0 && this->getHeader("Content-Length") == "") {
            throw HttpRequestException(400); // Content-Length header is required for POST/DELETE with body
        }
        else if (this->getHeader("Content-Length") != "") {
            int contentLength = stringToInt(this->getHeader("Content-Length"));
            if (contentLength != this->body.size()) {
                throw HttpRequestException(400); // Content-Length mismatch
            }
        }
    }
}

std::string HttpRequest::normalizeUri(const std::string &uri) {
    // Normalize the URI by removing duplicate slashes and ensuring it starts with a single slash
    std::string normalized = uri;
    while (normalized.find("//") != std::string::npos) {
        normalized.replace(normalized.find("//"), 2, "/");
    }
    if (normalized.empty() || normalized[0] != '/') {
        normalized = "/" + normalized;
    }
    return normalized;
}

