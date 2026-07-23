#pragma once
#include <string>
#include <unordered_map>
class Config
{
public:
    
    bool load(const std::string& path = FILE_NAME);
    std::string get(const std::string& key, const std::string& defaultValue = "") const;
    std::string serverUrl() const;

private:
    std::string m_serverUrl;
    std::unordered_map<std::string, std::string> values_;
    static constexpr const char* FILE_NAME = "/etc/print-agent/print-agent.conf";
    
};