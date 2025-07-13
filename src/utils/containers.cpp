#include "Webserv.hpp"

void printVec(const StringVec &vec)
{
    for (size_t i = 0; i < vec.size(); i++)
    {
        std::cout << vec[i] << std::endl;
    }
}

void printMap(const StringMap &map)
{
    StringMap::const_iterator it = map.begin();
    for (; it != map.end(); it++)
    {
        std::cout << it->first << " : " << it->second << std::endl;
    }
}
