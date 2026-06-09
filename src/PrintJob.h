#pragma once

#include <string>

struct PrintJob
{
    int jobId = 0;

    std::string printer;

    std::string user;

    std::string fileName;

    int copies = 0;
};