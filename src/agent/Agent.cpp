#include "Agent.h"

#include "parser/JournalReader.h"
#include "parser/JobParser.h"
#include "output/JsonWriter.h"

#include "config/Config.h"
#include "transport/HttpSender.h"
#include "transport/ZabbixSender.h"

#include <fstream>
#include <iostream>

void Agent::rebuild()
{
    JournalReader reader;

    auto messages =
        reader.readMessages();

    JobParser parser;

    auto jobs =
        parser.parse(messages);

    JsonWriter writer;

    if (!writer.save(jobs))
    {
        std::cerr
            << "Failed to save" << writer.FILE_NAME
            << std::endl;
    }
}

void Agent::update()
{
    JournalReader reader;

    auto messages =
        reader.readMessages();

    JobParser parser;

    auto jobs =
        parser.parse(messages);

    JsonWriter writer;

    if (!writer.update(jobs))
    {
        std::cerr
            << "Failed to update" << writer.FILE_NAME
            << std::endl;
    }
}

void Agent::send()
{
    Config config;

    JsonWriter writer;

    if (!config.load(".env"))
    {
        std::cerr
            << "Failed to load .env"
            << std::endl;

        return;
    }

    std::ifstream file(
        writer.FILE_NAME);

    if (!file.is_open())
    {
        std::cerr
            << "jobs.json not found"
            << std::endl;

        return;
    }

    nlohmann::json report;

    try
    {
        file >> report;
    }
    catch (...)
    {
        std::cerr
            << "Invalid" << writer.FILE_NAME
            << std::endl;

        return;
    }

    HttpSender sender;

    bool result =
        sender.send(
            config.get("SERVER_URL"),
            report);

    if (!result)
    {
        std::cerr
            << "Failed to send report"
            << std::endl;
    }
}

void Agent::sync()
{
    update();
    send();
}
void Agent::zabbixsend()
{
    Config config;

    JsonWriter writer;

    if (!config.load(".env"))
    {
        std::cerr
            << "Failed to load .env"
            << std::endl;
        return;
    }

    std::cout
        << "Server: "
        << config.get("ZABBIX_HOST")
        << std::endl;

    std::cout
        << "Port: "
        << config.get("ZABBIX_PORT")
        << std::endl;

    std::cout
        << "Item host: "
        << config.get("ZABBIX_ITEM_HOST")
        << std::endl;

    std::cout
        << "Item key: "
        << config.get("ZABBIX_ITEM_KEY")
        << std::endl;
    
    std::ifstream file(
        writer.FILE_NAME);

    if (!file.is_open())
    {
        std::cerr
            << "jobs.json not found"
            << std::endl;

        return;
    }
    

    nlohmann::json jobs;

    try
    {
        file >> jobs;
    }
    catch (...)
    {
        std::cerr
            << "Invalid" << writer.FILE_NAME
            << std::endl;
        return;
    }
    for (const auto& job : jobs)
    {
        std::cout
            << job.dump()
            << std::endl;
    }
    ZabbixSender sender;

    for (const auto& job : jobs)
    {
            bool result =
        sender.send(
            config.get("ZABBIX_HOST"),
            std::stoi(
                config.get("ZABBIX_PORT")),
            config.get("ZABBIX_ITEM_HOST"),
            config.get("ZABBIX_ITEM_KEY"),
            job.dump());
        
    if (!result)
    {
        std::cerr
            << "Failed to send job report"
            << std::endl;
    }
    }

}