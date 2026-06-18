#include "JsonWriter.h"

#include <fstream>

#include <nlohmann/json.hpp>

using json =
    nlohmann::json;

void JsonWriter::save(
    const std::vector<PrintJob>& jobs)
{
    json root =
        json::array();

    for (const auto& job : jobs)
    {
        root.push_back(
        {
            {"job_id", job.jobId},
            {"printer", job.printer},
            {"user", job.user},
            {"file_name", job.fileName},
            {"copies", job.copies}
        });
    }

    std::ofstream file(
        "jobs.json");

    file << root.dump(4);
}