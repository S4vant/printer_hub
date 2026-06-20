#pragma once
#include <string>
#include <cstdint>
class StateStorage
{
public:
    bool save(uint64_t timestamp);
    bool load(); 

    uint64_t getlastSent() const;
    

private:
    uint64_t m_lastsent = 0;
    static constexpr const char* FILE_NAME = "state.json";
};