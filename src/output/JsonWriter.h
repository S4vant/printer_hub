#pragma once

#include "PrintJob.h"

#include <vector>

class JsonWriter
{
public:

    nlohmann::json build(
        const std::vector<PrintJob>& jobs);

    bool save(
        const std::vector<PrintJob>& jobs);

    bool append(
        const std::vector<PrintJob>& jobs);
};