#pragma once

#include <string>
#include <vector>   

struct JournalEntry {
    std::string hostname;
    std::string comm;
    std::string syslogIdentifier;
    std::string message;
    
    uint64_t realtimeUsec = 0;
};
