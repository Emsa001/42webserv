#include "Webserv.hpp"

void ConfigParser::updateParents()
{
    for (size_t i = 0; i < this->blocks.size(); i++)
    {
        config_map &block = this->blocks[i];

        int level = block.at("blockLevel").getInt();

        if (level == 0)
        {
            this->root[block.at("blockName")] = block;
            continue;
        }
    }
}

config_map *ConfigParser::findParentBlock(int blockId, int level)
{
    config_map *parent = &this->root;

    if (blockId <= 0 || blockId > static_cast<int>(this->blocks.size()))
    {
        return parent;
    }

    if (this->block == NULL)
    {
        return parent;
    }

    for (int i = blockId - 1; i >= 0; i--)
    {
        if (i >= static_cast<int>(this->blocks.size()))
        {
            std::cerr << "Warning: Out-of-bounds access attempt at index " << i << std::endl;
            continue;
        }

        config_map::iterator it = this->blocks[i].find("blockLevel");
        if (it != this->blocks[i].end() && it->second.getType() == INT)
        {
            int foundLevel = it->second.getInt();
            if (foundLevel == level - 1)
            {
                return &this->blocks[i];
            }
        }
    }

    return parent;
}

void ConfigParser::updateParentBlock(config_map *parent, const std::string &blockName, int blockId)
{
    if (blockName == "location")
    {
        if (!parent->count("locations"))
            (*parent)["locations"] = ConfigValue(config_array());
        config_array locations = (*parent)["locations"];
        bool blockIdExists = false;

        for (size_t i = 0; i < locations.size(); ++i)
        {
            if (locations[i].getMap().at("blockId").getInt() == blockId)
            {
                locations[i] = *this->block;
                blockIdExists = true;
                break;
            }
        }

        if (!blockIdExists)
            locations.push_back(*this->block);

        (*parent)["locations"] = locations;
        return;
    }

    (*parent)[blockName] = *this->block;
}
