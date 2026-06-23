#include "ZabbixSender.h"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstdint>
#include <vector>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

bool ZabbixSender::send(
    const std::string& server,
    int port,
    const std::string& host,
    const std::string& key,
    const std::string& value,
    uint64_t& clock = 0)
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
    std::cout
    << "Connecting to "
    << server
    << ":"
    << port
    << std::endl;

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

    json item =
{
    {"host", host},
    {"key", key},
    {"value", value}
};

// Если timestamp передан
if (clock != 0)
{
    item["clock"] = clock;
}

json payload =
{
    {"request", "sender data"},
    {
        "data",
        json::array({ item })
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

    // Заголовок протокола
    packet.push_back('Z');
    packet.push_back('B');
    packet.push_back('X');
    packet.push_back('D');
    packet.push_back('T');
    packet.push_back(0x01);

    uint64_t payloadSize =
        payload.size();

    // Размер payload (little endian)
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
        {
            std::cerr
                << "Failed to send packet to Zabbix"
                << std::endl;

            return false;
        }

        totalSent += sent;
    }

    char header[13];

    ssize_t received =
        recv(
            socketFd,
            header,
            sizeof(header),
            MSG_WAITALL);

    if (received != sizeof(header))
    {
        std::cerr
            << "Failed to receive Zabbix response header"
            << std::endl;

        return false;
    }

    // Проверяем сигнатуру
    if (
        header[0] != 'Z' ||
        header[1] != 'B' ||
        header[2] != 'X' ||
        header[3] != 'D' ||
        header[4] != 'T' ||
        header[5] != 0x01)
    {
        std::cerr
            << "Invalid Zabbix response header"
            << std::endl;

        return false;
    }

    uint64_t responseSize = 0;

    for (int i = 0; i < 8; ++i)
    {
        responseSize |=
            static_cast<uint64_t>(
                static_cast<unsigned char>(
                    header[5 + i]))
            << (i * 8);
    }

    std::string response(
        responseSize,
        '\0');

    received =
        recv(
            socketFd,
            response.data(),
            responseSize,
            MSG_WAITALL);

    if (
        received !=
        static_cast<ssize_t>(
            responseSize))
    {
        std::cerr
            << "Failed to receive full Zabbix response"
            << std::endl;

        return false;
    }

    std::cout
        << "Zabbix response:"
        << std::endl
        << response
        << std::endl;

    try
    {
        auto jsonResponse =
            json::parse(response);

        std::cout
            << "Status: "
            << jsonResponse.value(
                   "response",
                   "unknown")
            << std::endl;

        if (
            jsonResponse.contains(
                "info"))
        {
            std::cout
                << "Info: "
                << jsonResponse["info"]
                << std::endl;
        }

        return
            jsonResponse.value(
                "response",
                "") == "success";
    }
    catch (...)
    {
        std::cerr
            << "Unable to parse Zabbix response"
            << std::endl;

        return false;
    }
}