#pragma once

#include <vector>
#include <string>
#include <cstdint>

class JournalReader
{
public:
    std::vector<std::string> readMessages();
    std::vector<std::string> ReadLastMassagesByTimestamp(uint64_t timestamp);
};