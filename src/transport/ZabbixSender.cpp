#include "ZabbixSender.h"

#include <arpa/inet.h>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

bool ZabbixSender::send(
    const std::string& server,
    int port,
    const std::string& host,
    const std::string& key,
    const std::string& value)
{
    int sock =
        socket(
            AF_INET,
            SOCK_STREAM,
            0);

    if (sock < 0)
        return false;

    sockaddr_in addr {};

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if (
        inet_pton(
            AF_INET,
            server.c_str(),
            &addr.sin_addr) <= 0)
    {
        close(sock);
        return false;
    }

    if (
        connect(
            sock,
            reinterpret_cast<sockaddr*>(&addr),
            sizeof(addr)) < 0)
    {
        close(sock);
        return false;
    }

    json payload =
    {
        {"request", "sender data"},
        {
            "data",
            json::array(
            {
                {
                    {"host", host},
                    {"key", key},
                    {"value", value}
                }
            })
        }
    };

    bool result =
        sendPacket(
            sock,
            payload.dump());

    close(sock);

    return result;
}