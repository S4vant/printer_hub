#include "JournalReader.h"

#include <systemd/sd-journal.h>
#include <ostream>
#include <iostream>
#include <assert.h>    // для функции assert
#include <algorithm>


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
JournalReader::ReadLastMessagesByTimestamp(
    uint64_t lastTimestamp)
{
    std::vector<std::string> messages;
    sd_journal* journal = nullptr;

    if (sd_journal_open(
            &journal,
            SD_JOURNAL_LOCAL_ONLY) < 0)
    {
        return messages;
    }

    if (sd_journal_seek_tail(journal) < 0)
    {
        sd_journal_close(journal);
        return messages;
    }

    // Переходим на последнюю запись
    sd_journal_previous(journal);

    SD_JOURNAL_FOREACH_BACKWARDS(journal)
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

        std::string message =
            field.substr(pos + 1);

        //
        // Ищем time-at-creation в любом сообщении.
        //  

        auto p =
        message.find(" time-at-creation=");

        if (p != std::string::npos)
        {
            p += strlen(" time-at-creation=");

            uint64_t createdAt =
                std::strtoull(
                    message.c_str() + p,
                    nullptr,
                    10);

            if (createdAt <= lastTimestamp)
            {
                break;
            }
        }

        messages.push_back(
            std::move(message));
    }

    sd_journal_close(journal);

    std::reverse(
        messages.begin(),
        messages.end());

    return messages;
}