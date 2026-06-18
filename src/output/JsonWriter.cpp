#include "JsonWriter.h"

#include <fstream>

#include <nlohmann/json.hpp>

using json =
    nlohmann::json;
// Сохранение в файл всего json
bool JsonWriter::save(
    const std::vector<PrintJob>& jobs)
{
    auto root =
        build(jobs);

    std::ofstream file(
        FILE_NAME);

    if (!file.is_open())
        return false;

    file << root.dump(4);

    return true;
}
// Единое место форматирование json
nlohmann::json JsonWriter::build(
    const std::vector<PrintJob>& jobs)
{
    nlohmann::json root =
        nlohmann::json::array();

    for (const auto& job : jobs)
    {
        root.push_back(
        {
            {"job_id", job.jobId},
            {"printer", job.printer},
            {"user", job.user},
            {"file_name", job.fileName},
            {"copies", job.copies},
            {"created_at", job.createdAt}
        });
    }

    return root;
}

bool JsonWriter::update(
    const std::vector<PrintJob>& jobs)
{
    using json = nlohmann::json;

    json root = json::array();

    {
        std::ifstream input(
            FILE_NAME);

        if (input.is_open())
        {
            try
            {
                input >> root;
            }
            catch (...)
            {
                root =
                    json::array();
            }
        }
    }

    uint64_t latestTime = 0;

    for (const auto& item : root)
    {
        if (!item.contains(
                "created_at"))
        {
            continue;
        }

        latestTime =
            std::max(
                latestTime,
                item["created_at"]
                    .get<uint64_t>());
    }

    for (const auto& job : jobs)
    {
        if (job.createdAt <= latestTime)
            continue;

        root.push_back(
        {
            {"job_id", job.jobId},
            {"printer", job.printer},
            {"user", job.user},
            {"file_name", job.fileName},
            {"copies", job.copies},
            {"created_at", job.createdAt}
        });
    }

    std::ofstream output(
        FILE_NAME);

    if (!output.is_open())
        return false;

    output << root.dump(4);

    return true;
}