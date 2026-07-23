#pragma once

#include "models/PrintJob.h"

#include <vector>

#include <nlohmann/json.hpp>
class JsonWriter
{
    // Единое место форматирование json по умолчанию
public:
    static constexpr const char* FILE_NAME = "/var/lib/print-agent/print_jobs.json";
    bool save(
        const std::vector<PrintJob>& jobs);

    bool update(
        const std::vector<PrintJob>& jobs);
private:

    nlohmann::json build(
        const std::vector<PrintJob>& jobs);
};