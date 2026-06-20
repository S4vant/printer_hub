#include "StateStorage.h"

#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

bool StateStorage::save(uint64_t timestamp)
{
    json root = 
    {
        {
            "last_sent_created_at", 
            timestamp
        }
    };

    std::ofstream file(
        FILE_NAME);

    if (!file.is_open())
        return false;

    file << root.dump(4);

    m_lastsent = timestamp;

    return true;
}

bool StateStorage::load(uint64_t timestamp)
{
    std::ifstream file(
        FILE_NAME);

    if (!file.is_open())
        return false;

    json root;
     
    file >> root;

    try 
    {
        m_lastsent = root.value("last_sent_created_at", 0);
        return true;
    } 
    catch (...) 
    {
        m_lastsent = 0; 
        return false;
    }
}

uint64_t StateStorage::getlastSent() const
{
    return m_lastsent;
}