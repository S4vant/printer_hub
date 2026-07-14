#pragma once

#include <vector>
#include <string>

class JournalReader
{
public:
    std::vector<std::string> readMessages();
    std::vector<std::string> readMessagesByTimestamp(uint64_t timestamp);
};