#pragma once

#include "PrintJob.h"

#include <vector>

class JsonWriter
{
public:

    void save(
        const std::vector<PrintJob>& jobs);
};