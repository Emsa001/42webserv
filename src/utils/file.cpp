#include "Webserv.hpp"

std::string getLastModifiedTime(const std::string &filePath)
{
    struct stat fileInfo;
    if (stat(filePath.c_str(), &fileInfo) != 0)
    {
        return "Unknown";
    }

    char buffer[20];
    std::tm timeinfo;
    localtime_r(&fileInfo.st_mtime, &timeinfo);

    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeinfo);
    return std::string(buffer, buffer + std::strlen(buffer));
}

std::string readFileContent(const std::string &filePath)
{
    std::ifstream file(filePath.c_str(), std::ios::in | std::ios::binary);
    if (!file)
        return "";

    std::ostringstream contents;
    contents << file.rdbuf();
    return contents.str();
}

bool isDirectory(const std::string &filePath)
{
    struct stat fileInfo;
    if (stat(filePath.c_str(), &fileInfo) != 0)
    {
        return false;
    }
    return S_ISDIR(fileInfo.st_mode);
}

bool fileExists(const std::string &filePath)
{
    struct stat fileInfo;
    return (stat(filePath.c_str(), &fileInfo) == 0);
}

FileData getFileData(const std::string &path)
{
    FileData fileData;
    fileData.path = path;
    fileData.name = path.substr(path.find_last_of("/") + 1);
    fileData.exists = fileExists(path);

    if (!fileData.exists)
    {
        fileData.isDirectory = false;
        fileData.lastModified = "";
        return fileData;
    }

    fileData.isDirectory = isDirectory(path);

    if (!fileData.isDirectory)
    {
        fileData.lastModified = getLastModifiedTime(path);
    }

    return fileData;
}

std::vector<FileData> getFiles(const std::string &path)
{
    std::vector<FileData> files;
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(path.c_str())) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            if (ent->d_name[0] == '.')
                continue;
            std::string fullPath = path + "/" + ent->d_name;
            FileData fileData = getFileData(fullPath);
            files.push_back(fileData);
        }
        closedir(dir);
    }
    return files;
}
