#pragma once

#include <string>

#include <nlohmann/json.hpp>

class HttpSender
{
public:
    bool send(
        const std::string& url,
        const nlohmann::json& json);
};