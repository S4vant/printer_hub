#include "JournalReader.h"

#include <systemd/sd-journal.h>
#include <ostream>
#include <iostream>
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
    std::cout << messages.back() << std::endl;
    sd_journal_close(journal);

    return messages;
}