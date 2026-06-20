#pragma once
#include <string>
#include <c
class StateStorage
{
public:
    bool save();
    bool load(uint64_t timestamp); 

    uint64_t getlastSent() const;
    

private:
    uint64_t m_lastsent = 0;
    static constexpr const char* FILE_NAME = "state.json";
};