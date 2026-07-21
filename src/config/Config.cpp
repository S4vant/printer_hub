#include "Config.h"

#include <fstream>
#include <algorithm>
#include <string>

// При отсутствии прямого пути к файлу конфигурации, используется по умолчанию 21.07
bool Config::load(const std::string& path = FILE_NAME)
{
    std::ifstream file(path);

    if (!file.is_open())
    {
        // дебаг
        std::cout << "Failed to open config file (Config::load)" << std::endl;
        return false;
    }
        

    std::string line;

    while (std::getline(file, line))
    {
        if (line.empty())
            continue;

        if (line[0] == '#')
            continue;

        auto pos = line.find('=');

        if (pos == std::string::npos)
            continue;

        std::string key =
            line.substr(0, pos);

        std::string value =
            line.substr(pos + 1);

        values_[key] = value;
    }

    return true;
}

std::string Config::get(
    const std::string& key,
    const std::string& defaultValue) const
{
    auto it = values_.find(key);

    if (it == values_.end())
        return defaultValue;

    return it->second;
}