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
void HttpRequest::parse() {
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
    if (totalHeaderSize > this->maxHeaderSize) {
        throw HttpRequestException(414);
    }

    std::istringstream lineStream(line);
    lineStream >> this->method >> this->uri >> this->version;
    this->url = new HttpURL(this->uri);

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
            this->headers[key] = value;
        }

        if (totalHeaderSize > this->maxHeaderSize) {
            throw HttpRequestException(431);
        }
    }

    // 6. For POST and DELETE methods, assign the body and check its size against the maximum allowed
    if (this->method == "POST" || this->method == "DELETE") {
        this->body = bodyPart;
        if (this->body.size() > this->maxBodySize) {
            throw HttpRequestException(413);
        }
    }
}
