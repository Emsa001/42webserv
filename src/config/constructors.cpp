#include "Webserv.hpp"

void ConfigParser::init()
{
    this->block = NULL;
    this->blockId = 0;
    this->indent = 0;
    this->expectedIndent = -1;

    this->schema = createSchema();

    this->ln = 0;
}

ConfigParser::ConfigParser()
{
    this->init();
    // std::cout << "Config is created" << std::endl;
    this->file.open("conf/default.yml");
}

ConfigParser::ConfigParser(std::string const &filename)
{
    this->init();
    // std::cout << "Config is created" << std::endl;
    this->file.open(filename.c_str());
}

ConfigParser::~ConfigParser()
{
    // std::cout << "Config destructor called, resources freed successfully." <<
    // std::endl;
}
