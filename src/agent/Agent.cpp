#include "Agent.h"



#include <fstream>
#include <iostream>
#include <stdexcept>

Agent::Agent()
{
    if (!config_.load())
        throw std::runtime_error("Failed to load config_");
}

void Agent::rebuild()
{

    auto messages =
        reader_.readMessages();


    auto jobs =
        parser_.parse(messages);


    bool result = writer_.save(jobs);
    if (!result)
    {
        std::cerr
            << "Failed to save" << writer_.FILE_NAME
            << std::endl;
    }
}

void Agent::update()
{
    // Добавление в json новых job
    state_.load();
    uint64_t lastUpdate = state_.getLastSent();

    std::cout << "at Agent::update "<< "Last update: " << lastUpdate << std::endl;

    auto messages =
        reader_.ReadLastMessagesByTimestamp(lastUpdate);


    auto jobs =
        parser_.parse(messages);


    if (!writer_.update(jobs))
    {
        std::cerr
            << "Failed to update" << writer_.FILE_NAME
            << std::endl;
    }
}

void Agent::send()
{

    std::ifstream file(
        writer_.FILE_NAME);

    if (!file.is_open())
    {
        std::cerr
            << writer_.FILE_NAME << " not found"
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
            << "Invalid" << writer_.FILE_NAME
            << std::endl;

        return;
    }

    HttpSender sender;

    bool result =
        sender.send(
            config_.get("SERVER_URL"),
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

// отправка в заббикс Универсальная функция

bool Agent::sendToZabbix(
    const std::string& key,
    const std::string& value,
    uint64_t clock)
{

    std::cout
        << "Server: "
        << config_.get("ZABBIX_HOST")
        << std::endl;

    std::cout
        << "Port: "
        << config_.get("ZABBIX_PORT")
        << std::endl;

    std::cout
        << "Item host: "
        << config_.get("ZABBIX_ITEM_HOST")
        << std::endl;

    ZabbixSender sender;

    return sender.send(
        config_.get("ZABBIX_HOST"),
        std::stoi(config_.get("ZABBIX_PORT")),
        config_.get("ZABBIX_ITEM_HOST"),
        key,
        value,
        clock);
}
// 
void Agent::zabbixsend_all()
{
    // Отправка в заббикс всех job из дампа в json
    //в load можно добавить путь свой путь к файлу

    std::cout
        << "Item key: "
        << config_.get("ZABBIX_ITEM_KEY")
        << std::endl;
    
    std::ifstream file(
        writer_.FILE_NAME);

    if (!file.is_open())
    {
        std::cerr
            << "Failed to open" << writer_.FILE_NAME << " not found"
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
            << "Invalid" << writer_.FILE_NAME
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

    for (const auto& job : jobs)
    {
        std::cout
            << job.dump()
            << std::endl;
    }
    
    uint64_t maxtimestamp = 0;
    for (const auto& job : jobs)
    {
            bool result =  sendToZabbix(
            config_.get("ZABBIX_ITEM_KEY"),
            job.dump(),
            job["created_at"].get<uint64_t>());  
    if (!result)
    {
        std::cerr
            << "Failed to send job report"
            << std::endl;
    }
    else
    {
        maxtimestamp = std::max(maxtimestamp, job["created_at"].get<uint64_t>());
    }
    }

    std::cout
        << "Max timestamp: "
        << maxtimestamp
        << std::endl;
    state_.setLastSent(maxtimestamp);
    state_.save();

}
void Agent::zabbixsend_new()
{
    // Отправка в заббикс всех job из дампа в json
    //в load можно добавить путь свой путь к файлу

    if (!state_.load())
    {
        std::cerr
            << "Failed to load state_"
            << std::endl;

        return;
    }
    std::cout
        << "Item key: "
        << config_.get("ZABBIX_ITEM_KEY")
        << std::endl;

    std::ifstream file(
        riter.FILE_NAME);

    if (!file.is_open())
    {
        std::cerr
            << "Failed to open "
            << writer_.FILE_NAME
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
        state_.getLastSent();

    uint64_t newestSent =
        lastSent;

    

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
            sendToZabbix(
                config_.get("ZABBIX_ITEM_KEY"),
                job.dump(),
                job["created_at"].get<uint64_t>());
                

        if (!result)
        {
            std::cerr
                << "Failed to send job "
                << job.value("job_id", -1)
                << std::endl;

            break;
        }
            else
        {
            newestSent = std::max(newestSent, job["created_at"].get<uint64_t>());
        }

        newestSent =
            createdAt;

        ++sentCount;
    }

    if (newestSent > lastSent)
    {
        state_.setLastSent(newestSent);
        if (!state_.save())
        {
            std::cerr
                << "Failed to save state_"
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


void Agent::help()
{
    std::cout
        << "help: show this help message"
        << std::endl;

    std::cout
        << "rebuild: rebuild json file from journal messages"
        << std::endl;

    std::cout
        << "update: update json file from journal messages"
        << std::endl;

    std::cout
        << "zabbixsend_all: send all jobs to zabbix"
        << std::endl;
    std::cout
        << "zabbixsend_new: send new jobs to zabbix and update state_"
        << std::endl;
    std::cout
        << "healthcheck: check health"
        << std::endl;
}

void Agent::exception()
{
    std::cout
        << "Command not found. Use help if you need help"
        
        << std::endl;
}

void Agent::zabbixsend_healthcheck()
{
    std::cout
        << "OK"
        << std::endl;
    // Отправка в заббикс всех job из дампа в json
    //в load можно добавить путь свой путь к файлу

    sendToZabbix(
    config_.get("ZABBIX_HEALTH_KEY"),
    "1",
    std::time(nullptr));

}