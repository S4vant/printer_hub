#include "JournalReader.h"

#include <systemd/sd-journal.h>
#include <ostream>
#include <iostream>
#include <assert.h>    // для функции assert
#include <cstdint>

std::vector<std::string>
JournalReader::readMessages()
{
    std::vector<std::string> messages;

    sd_journal* journal = nullptr;

    if (sd_journal_open(
            &journal,
            SD_JOURNAL_LOCAL_ONLY) < 0)
    {
        return messages;
    }

    // sd_journal_add_match(
    //     journal,
    //     "_SYSTEMD_UNIT=cups.service",
    //     0);

    SD_JOURNAL_FOREACH(journal)
    {
        const void* data;
        size_t length;

        if (sd_journal_get_data(
                journal,
                "MESSAGE",
                &data,
                &length) < 0)
        {
            continue;
        }

        std::string field(
            static_cast<const char*>(data),
            length);

        auto pos = field.find('=');

        if (pos == std::string::npos)
            continue;

        messages.push_back(
            field.substr(pos + 1));
    }
    sd_journal_close(journal);

    return messages;
}
std::vector<std::string>
JournalReader::ReadLastMassagesByTimestamp(uint64_t timestamp)
{
    std::vector<std::string> messages;
    sd_journal* journal = nullptr;

    if (sd_journal_open(
            &journal,
            SD_JOURNAL_LOCAL_ONLY) < 0)
    {
        return messages;
    }



    r = sd_journal_seek_tail(journal);
    if (r < 0) {
        std::cerr << "Ошибка перемещения в конец логов: " << std::strerror(-r) << std::endl;
        sd_journal_close(journal);
        return 1;
    }

    bool found = false;

    SD_JOURNAL_FOREACH_BACKWARD(journal)
    {
        const void* data;
        size_t length;

        if (sd_journal_get_data(
                journal,
                "MESSAGE",
                &data,
                &length) < 0)
        {
            continue;
        }

        std::string field(
            static_cast<const char*>(data),
            length);

        auto pos = field.find('=');

        if (pos == std::string::npos)
            continue;
        
        uint64_t timestampJob = std::stoull(field.substr(pos + 1));

        if (timestampJob < timestamp)
            break;
        messages.push_back(
            field.substr(pos + 1));
    }
    sd_journal_close(journal);

    return messages;
}