#include "HttpResponse.hpp"


/*

    Any HTTP/1.1 message containing an entity-body SHOULD include a
    Content-Type header field defining the media type of that body. If
    and only if the media type is not given by a Content-Type field, the
    recipient MAY attempt to guess the media type via inspection of its
    content and/or the name extension(s) of the URI used to identify the
    resource. If the media type remains unknown, the recipient SHOULD
    treat it as type "application/octet-stream".

    https://www.rfc-editor.org/rfc/rfc2616#section-7.2.1

*/

std::string HttpResponse::getMimeType(const std::string &path) {
    static StringMap mimeTypes;
    if (mimeTypes.empty()) {
        mimeTypes[".html"] = "text/html";
        mimeTypes[".htm"] = "text/html";
        mimeTypes[".css"] = "text/css";
        mimeTypes[".js"] = "application/javascript";
        mimeTypes[".json"] = "application/json";
        mimeTypes[".xml"] = "application/xml";
        mimeTypes[".png"] = "image/png";
        mimeTypes[".jpg"] = "image/jpeg";
        mimeTypes[".jpeg"] = "image/jpeg";
        mimeTypes[".gif"] = "image/gif";
        mimeTypes[".bmp"] = "image/bmp";
        mimeTypes[".ico"] = "image/vnd.microsoft.icon";
        mimeTypes[".svg"] = "image/svg+xml";
        mimeTypes[".webp"] = "image/webp";
        mimeTypes[".mp3"] = "audio/mpeg";
        mimeTypes[".wav"] = "audio/wav";
        mimeTypes[".mp4"] = "video/mp4";
        mimeTypes[".mpeg"] = "video/mpeg";
        mimeTypes[".avi"] = "video/x-msvideo";
        mimeTypes[".pdf"] = "application/pdf";
        mimeTypes[".zip"] = "application/zip";
        mimeTypes[".gz"] = "application/gzip";
        mimeTypes[".tar"] = "application/x-tar";
        mimeTypes[".rar"] = "application/vnd.rar";
        mimeTypes[".7z"] = "application/x-7z-compressed";
        mimeTypes[".txt"] = "text/plain";
        mimeTypes[".csv"] = "text/csv";
        mimeTypes[".php"] = "application/x-httpd-php";
        mimeTypes[".wasm"] = "application/wasm";
    }

    size_t pos = path.find_last_of('.');
    if (pos != std::string::npos) {
        std::string ext = path.substr(pos);
        StringMap::iterator it = mimeTypes.find(ext);
        if (it != mimeTypes.end()) {
            return it->second;
        }
    }
    return "application/octet-stream";
}
