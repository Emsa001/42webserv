#include "HttpResponse.hpp"

void HttpResponse::directoryListing(const FileData &fileData) {
    std::string listing = "<html><head><title>Index of " + fileData.path + "</title></head><body><h1>Index of " + fileData.path  + "</h1><hr><pre>";

    listing += "<a href=\"../\">../</a>\n";

    std::vector<FileData> files = getFiles(fileData.path);
    std::string path = this->request->getURL()->getPath();

    for (size_t i = 0; i < files.size(); i++) {
        if (files[i].isDirectory) {
            listing += "<a href=\"" + path + "/" + files[i].name + "/\">./" + files[i].name + "/</a>\n";
        } else {
            listing += "<a href=\"" + path + "/" + files[i].name + "\">./" + files[i].name + "</a>\n";
        }
    }

    listing += "</pre><hr></body></html>";

    this->body = listing;
    this->setStatusCode(200);
    this->setHeader("Content-Type", "text/html");
    this->setHeader("Content-Length", intToString(this->body.size()));
    this->build();
}

void HttpResponse::buildBody(FileData &fileData, const HttpRequest *request) {

    // std::cout << "Setting body" << std::endl;
    // std::cout << "File exists: " << fileData.exists << std::endl;
    // std::cout << "Is directory: " << fileData.isDirectory << std::endl;
    // std::cout << "Listing: " << this->listing << std::endl;
    // std::cout << "Path: " << fileData.path << std::endl;

    if (fileData.exists == false) {
        this->respondStatusPage(404);
        return ;
    }

    if(fileData.isDirectory){
        if(this->listing == true){
            this->directoryListing(fileData);
            return ;
        }
        this->respondStatusPage(403);
        return ;
    }

    if(this->cgi){
        Cgi cgi;
        cgi.execute(fileData.path, this, request);
        return ;
    }

    this->body = fileData.getContent();
    this->setStatusCode(200);
    // this->setHeader("Last-Modified", fileData.lastModified); << I'm not handling caching, no way
    this->setHeader("Content-Type", HttpResponse::getMimeType(fileData.path));
    this->setHeader("Content-Length", intToString(this->body.size()));
    this->build();
}
