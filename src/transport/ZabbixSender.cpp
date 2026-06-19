#include "ZabbixSender.h"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstdint>
#include <vector>

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

    sockaddr_in addr{};

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

bool ZabbixSender::sendPacket(
    int socketFd,
    const std::string& payload)
{
    std::vector<char> packet;

    // ZBXD\1
    packet.push_back('Z');
    packet.push_back('B');
    packet.push_back('X');
    packet.push_back('D');
    packet.push_back(0x01);

    uint64_t payloadSize =
        payload.size();

    // длина payload в little-endian
    for (int i = 0; i < 8; ++i)
    {
        packet.push_back(
            static_cast<char>(
                (payloadSize >> (i * 8)) & 0xFF));
    }

    packet.insert(
        packet.end(),
        payload.begin(),
        payload.end());

    size_t totalSent = 0;

    while (totalSent < packet.size())
    {
        ssize_t sent =
            ::send(
                socketFd,
                packet.data() + totalSent,
                packet.size() - totalSent,
                0);

        if (sent <= 0)
            return false;

        totalSent += sent;
    }

    // читаем ответ сервера
    char response[1024];

    ssize_t received =
        recv(
            socketFd,
            response,
            sizeof(response),
            0);
    std::cout << std::string(response, received)
          << std::endl;
    return received > 0;
}