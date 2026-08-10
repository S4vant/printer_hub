#include "ZabbixSender.h"

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstdint>
#include <cstring>
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
    uint64_t timestamp)
{
    int sock =
        socket(
            AF_INET,
            SOCK_STREAM,
            0);

    if (sock < 0)
        return false;

    std::cout
        << "Connecting to "
        << server
        << ":"
        << port
        << std::endl;

    addrinfo hints{};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    std::string portString = std::to_string(port);
    addrinfo* result = nullptr;
    int status = getaddrinfo(
        server.c_str(),
        portString.c_str(),
        &hints,
        &result);

    if (status != 0)
    {
        std::cerr
            << "Failed to resolve Zabbix host '"
            << server
            << "': "
            << gai_strerror(status)
            << std::endl;
        close(sock);
        return false;
    }

    bool connected = false;

    for (addrinfo* entry = result; entry != nullptr; entry = entry->ai_next)
    {
        if (entry->ai_family != AF_INET)
            continue;

        sockaddr_in addr{};
        std::memcpy(&addr, entry->ai_addr, entry->ai_addrlen);
        addr.sin_port = htons(port);

        if (
            connect(
                sock,
                entry->ai_addr,
                entry->ai_addrlen) == 0)
        {
            connected = true;
            break;
        }
    }

    freeaddrinfo(result);

    if (!connected)
    {
        std::cerr
            << "Failed to connect to Zabbix host '"
            << server
            << "'"
            << std::endl;
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
if (timestamp != 0)
{
    item["clock"] = timestamp;
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
        header[4] != 0x01)
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