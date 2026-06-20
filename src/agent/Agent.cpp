#include "Agent.h"

#include "parser/JournalReader.h"
#include "parser/JobParser.h"
#include "output/JsonWriter.h"

#include "config/Config.h"
#include "transport/HttpSender.h"
#include "transport/ZabbixSender.h"
#include "data/StateStorage.h"

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
    // Добавление в json новых job
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
            << writer.FILE_NAME << " not found"
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
void Agent::zabbixsend_all()
{
    // Отправка в заббикс всех job из дампа в json

    JsonWriter writer;
    Config config;
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
            << "Failed to open" << writer.FILE_NAME << " not found"
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
void Agent::zabbixsend_new()
{
    Config config;

    if (!config.load(".env"))
    {
        std::cerr
            << "Failed to load .env"
            << std::endl;

        return;
    }

    StateStorage state;

    if (!state.load())
    {
        std::cerr
            << "Failed to load state"
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
        JsonWriter::FILE_NAME);

    if (!file.is_open())
    {
        std::cerr
            << "Failed to open "
            << JsonWriter::FILE_NAME
            << std::endl;

        return;
    }

    nlohmann::json jobs;

    try
    {
        file >> jobs;
    }
    catch (const std::exception& e)
    {
        std::cerr
            << "Invalid JSON: "
            << e.what()
            << std::endl;

        return;
    }

    if (!jobs.is_array())
    {
        std::cerr
            << "jobs.json is not array"
            << std::endl;

        return;
    }

    if (jobs.empty())
    {
        std::cout
            << "No jobs found"
            << std::endl;

        return;
    }

    std::sort(
        jobs.begin(),
        jobs.end(),
        [](const nlohmann::json& a,
            const nlohmann::json& b)
            {
                return a["created_at"].get<uint64_t>()
                    < b["created_at"].get<uint64_t>();
            });

    uint64_t lastSent =
        state.getLastSent();

    uint64_t newestSent =
        lastSent;

    ZabbixSender sender;

    size_t sentCount = 0;

    for (const auto& job : jobs)
    {
        uint64_t createdAt;

        try
        {
            createdAt =
                job.at("created_at")
                    .get<uint64_t>();
        }
        catch (...)
        {
            std::cerr
                << "Job without created_at skipped"
                << std::endl;

            continue;
        }

        if (createdAt <= lastSent)
            continue;

        std::cout
            << "Sending job "
            << job.value("job_id", -1)
            << std::endl;

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
                << "Failed to send job "
                << job.value("job_id", -1)
                << std::endl;

            break;
        }

        newestSent =
            createdAt;

        ++sentCount;
    }

    if (newestSent > lastSent)
    {
        state.setLastSent(newestSent);
        if (!state.save())
        {
            std::cerr
                << "Failed to save state"
                << std::endl;

            return;
        }
    }

    std::cout
        << "Successfully sent "
        << sentCount
        << " new jobs"
        << std::endl;
}
