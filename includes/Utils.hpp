#ifndef UTILS_HPP
#define UTILS_HPP

// String operations
void trim(std::string &s);
std::string trimChar(const std::string &str, char c);
std::string intToString(int i);
int stringToInt(const std::string &s);
StringVec split(const std::string &s, char delim);
StringVec splitFirst(const std::string &s, char delim);
std::string toLower(const std::string &str);

// File operations
std::string readFileContent(const std::string &filePath);

struct FileData {
    std::string lastModified;
    std::string path;
    std::string name;
    size_t size;

    bool isDirectory;
    bool exists;

    std::string getContent() {
        std::string content = readFileContent(this->path);
        this->size = content.size();
        return content;
    }
};

FileData getFileData(const std::string &path);
std::vector<FileData> getFiles(const std::string &path);
std::string getLastModifiedTime(const std::string &filePath);
bool isDirectory(const std::string &filePath);
bool fileExists(const std::string &path);

// Containers
void printVec(const StringVec &vec);
void printMap(const StringMap &map);

#endif

