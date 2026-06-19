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
            << "Failed to save jobs.json"
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
            << "Failed to update jobs.json"
            << std::endl;
    }
}

void Agent::send()
{
    Config config;

    if (!config.load(".env"))
    {
        std::cerr
            << "Failed to load .env"
            << std::endl;

        return;
    }

    std::ifstream file(
        "jobs.json");

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
            << "Invalid jobs.json"
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

    ZabbixSender sender;

    if (!sender.send(
        config.get("ZABBIX_SERVER"),
        std::stoi(
            config.get("ZABBIX_PORT")),
        config.get("ZABBIX_HOST"),
        config.get("ZABBIX_KEY"),
        report.dump());):

    {
        std::cerr
            << "Failed to send zabbix report"
            << std::endl;
    }

}