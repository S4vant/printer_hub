#pragma once
#include <string>
class Config
{
public:
    bool load(const std::string& path);
    std::string get(const std::string& key, const std::string& defaultValue) const;
    std::string serverUrl() const;

private:
    std::string m_serverUrl;
};