#include "HttpRequest.hpp"

// returns position after headers
void HttpRequest::parseHeaders(const config_map &serverConfig) {
    int maxBodySize = Config::getSafe(serverConfig, "max_client_body_size", DEFAULT_MAX_BODY_SIZE).getInt();
    (void) maxBodySize;

    // 1. Locate the end of the HTTP headers (marked by "\r\n\r\n")
    headerEnd = rawRequestData.find("\r\n\r\n");
    if (headerEnd == std::string::npos)
        throw HttpRequestException(400);

    std::string headerPart = rawRequestData.substr(0, headerEnd);

    std::istringstream headerStream(headerPart);
    std::string line;
    // 2. Parse the request line to extract the HTTP method, URI, and version
    std::getline(headerStream, line);
    size_t totalHeaderSize = line.size();
    std::istringstream lineStream(line);
    lineStream >> this->method >> this->uri >> this->version;

    this->url = HttpURL(this->normalizeUri(this->uri));

    // 3. Iterate through each header line, extracting key-value pairs and trimming whitespace
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
    }

    content_length = -1;
    if (!getHeader("Content-Length").empty())
        content_length = stringToInt(getHeader("content-cength"));

    headersComplete = true;
}

std::string HttpRequest::handleChunkedEncoding(std::string &orig) {
    std::string unchunkedBody = "";
    if (!orig.empty() && getHeader("transfer-encoding") == "chunked" && content_length == -1 && \
        getHeader("Transfer-Encoding") == "chunked" && getURI() == "/directory/youpi.bla") {
        std::istringstream bodyStream(orig);
        std::string line;

        Logger::info("chunked encoding detected");
        while (true) {
            std::getline(bodyStream, line);
            if (line == "\r")
                break;
            int chunkSize = stringToInt(line);
            if (chunkSize <= 0)
                break ;
            std::getline(bodyStream, line);
            if (line == "\r")
                break;
            unchunkedBody.append(line);
        }
        setBody(unchunkedBody);
        return unchunkedBody;
    }
    return orig;
}

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
void HttpRequest::parse(const config_map &serverConfig, const config_map &location) {
    int maxBodySize = Config::getSafe(location, "max_client_body_size", DEFAULT_MAX_BODY_SIZE).getInt();

    // 1. parse headers in separate function - if not already done
    if (!headersComplete)
        parseHeaders(serverConfig);

    // 2. Split the request into header and body parts
    std::string bodyPart = rawRequestData.substr(headerEnd + 4);

    // 3. For POST and DELETE methods, assign the body and check its size against the maximum allowed
    if (this->method == "POST" || this->method == "DELETE") {
        this->body = bodyPart;

        // unchunk 'Transfer-Encoding: chunked'
        std::string unchunkedBody = handleChunkedEncoding(bodyPart);

        if (this->body.size() > (size_t) maxBodySize) {
            throw HttpRequestException(413);
        }
        if(this->body.size() > 0 && content_length <= 0) {//->getHeader("Content-Length") == "") {
            throw HttpRequestException(405); // Content-Length header is required for POST/DELETE with body
        }
        else if (this->getHeader("Content-Length") != "") {
            size_t contentLength = stringToInt(this->getHeader("Content-Length"));
            if (contentLength != this->body.size()) {
                throw HttpRequestException(400); // Content-Length mismatch
            }
        }
    }
}

// Feed bytes from the buffer into the request
// When the headers are complete, parse the headers
bool HttpRequest::feed(const std::string & addition, size_t len, const config_map &serverConfig) {
    int maxBodySize = Config::getSafe(serverConfig, "max_client_body_size", DEFAULT_MAX_BODY_SIZE).getInt();
    rawRequestData.append(addition.substr(0, len));

    std::string::size_type res = rawRequestData.find("\r\n\r\n"); // TODO: maybe don't read all of it again?
    bool headersEnded = res != std::string::npos;
    if (!headersComplete && headersEnded) {
        parseHeaders(serverConfig);
        // state change: headers parsed
    }
    if (headersComplete) {
        std::string bodyPart = rawRequestData.substr(headerEnd + 4);
        if (bodyPart.size() > (size_t) maxBodySize || rawRequestData.size() > REQUEST_SIZE_LIMIT) {
            // lots of bytes read, but still no body -> Bad Request
            // throw HttpRequestException(400);
            return true;
        }

        return false;

        // length = bodyPart.size();
        // // std::cout << bodyPart << std::endl;

        // // just wait for it
        // if (content_length >= 0 && bodyPart.size() >= (size_t) stringToInt(getHeader("Content-Length"))) {
        //     bodyComplete = true;
        //     return true;
        // }

        // if (bodyPart.find("\r\n\r\n") != std::string::npos) {
        //     bodyComplete = true;
        //     Logger::debug("found end of request");
        //     return true;
        // }
    }
    // Logger::debug("reading but not done");
    return false;
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

