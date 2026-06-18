#pragma once

#include <string>
#include <cstdint>

struct PrintJob
{
    int jobId = 0;

    std::string printer;

    std::string user;

    std::string fileName;
    
    uint64_t createdAt = 0;

    int copies = 0;
};