#pragma once

#include <string>

class ZabbixSender
{
public:

    bool send(
        const std::string& server,
        int port,
        const std::string& host,
        const std::string& key,
        const std::string& value,
        uint64_t& timestamp = 0);

private:

    bool sendPacket(
        int socketFd,
        const std::string& payload);
};