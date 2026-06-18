#pragma once

#include "PrintJob.h"

#include <unordered_map>
#include <vector>
#include <string>

class JobParser
{
public:

    std::vector<PrintJob>
    parse(
        const std::vector<std::string>& messages);
};