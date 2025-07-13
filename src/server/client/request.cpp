#include "Webserv.hpp"

// Simple header validation: checks for a valid HTTP request line
// static bool validateHeaders(const std::string& headers) {
//     size_t firstLineEnd = headers.find("\r\n");
//     if (firstLineEnd == std::string::npos) return false;
//     std::string requestLine = headers.substr(0, firstLineEnd);
//     // Basic check: METHOD SP URI SP HTTP/VERSION
//     size_t methodEnd = requestLine.find(' ');
//     if (methodEnd == std::string::npos) return false;
//     size_t uriEnd = requestLine.find(' ', methodEnd + 1);
//     if (uriEnd == std::string::npos) return false;
//     if (requestLine.find("HTTP/") == std::string::npos) return false;
//     return true;
// }

// int Server::readBytes(int fd, int index, char *buffer){
//     const int bytes_read = recv(fd, buffer, sizeof(buffer), 0);

//     if (bytes_read < 0 && errno != EWOULDBLOCK && errno != EAGAIN) {
//         Logger::error("Recv error on fd " + intToString(fd) + ", closing connection.");
//         this->removeClient(index);
//     }

//     if(bytes_read == 0){
//         this->removeClient(index);
//     }

//     return bytes_read;
// }

// ClientRequestState *Server::readChunk(int fd, int index, char *buffer) {
//     int bytes_read = this->readBytes(fd, index, buffer);
//     if (bytes_read <= 0) return NULL;

//     ClientRequestState &state = requestStates[fd];
//     state.buffer.append(buffer, bytes_read);

//     // Optional: prevent abuse with a hard limit for a chunk
//     if (state.buffer.size() > HARD_BUFFER_LIMIT) {
//         Logger::warning("Request exceeded hard buffer limit on fd " + intToString(fd));
        
//         HttpRequest dummyRequest(state.buffer, this->config);
//         HttpResponse response(&dummyRequest, this->config);
//         response.respondStatusPage(400);

//         this->removeClient(index);
//         return NULL;
//     }

//     size_t headerEnd = state.buffer.find("\r\n\r\n");
//     if (headerEnd != std::string::npos && !state.headersParsed) {
//         std::string headers = state.buffer.substr(0, headerEnd);

//         // Validate headers before proceeding
//         if (!validateHeaders(headers)) {
//             Logger::warning("Malformed HTTP headers on fd " + intToString(fd));
            
//             HttpRequest dummyRequest(state.buffer, this->config);
//             HttpResponse response(&dummyRequest, this->config);
//             response.respondStatusPage(400);

//             this->removeClient(index);
//             return NULL;
//         }

//         state.headersParsed = true;

//         // Soft parse just Content-Length to know when to stop reading
//         size_t clPos = headers.find("Content-Length:");

//         if (clPos != std::string::npos) {
//             size_t start = headers.find_first_of("0123456789", clPos);
//             if (start != std::string::npos) {
//                 state.contentLength = stringToInt(headers.substr(start));
//             }
//         } else {
//             state.contentLength = 0;
//         }

//         // Compute total required bytes: headers + body
//         state.expectedSize = headerEnd + 4 + state.contentLength;
//     }

//     // All data we need is available
//     if (state.headersParsed && state.buffer.size() >= state.expectedSize) {
//         return &state;
//     }

//     // Still waiting for more data
//     return NULL; 
// }


// TODO: This part will be dont by SocketHandler, Server functionality will be limited to handleRespose method
// void Server::handleClientRead(size_t index) {
//     int fd = this->fds[index].fd;
//     char buffer[READ_BUFFER_SIZE];
    
//     ClientRequestState *state = this->readChunk(fd, index, buffer);
//     if(state == NULL) return;

//     if (state->headersParsed) {
//         size_t totalRequired = state->buffer.find("\r\n\r\n") + 4 + state->contentLength;

//         if (state->buffer.size() >= totalRequired) {
//             wHttpRequest request(state->buffer, this->config);
//             request.parse();
//             HttpResponse response = this->handleResponse(&request);

//             if (response.getHeader("Connection") == "close") {
//                 this->removeClient(index);
//             }

//             requestStates.erase(fd);
//         }
//     }
// }

