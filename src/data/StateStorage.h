#pragma once
#include <string>
#include <cstdint>
class StateStorage
{
public:
    bool save();
    bool load(); 
    bool addLastSent(uint64_t timestamp);

    uint64_t getLastSent() const;
    bool setLastSent(uint64_t timestamp);
    

private:
    uint64_t m_lastsent = 0;
    static constexpr const char* FILE_NAME = "/var/lib/print-agent/state.json";
};