#include "JournalReader.h"

#include <iostream>
#include <cstring>

JournalReader::JournalReader()
    : journal(nullptr)
{
}

JournalReader::~JournalReader()
{
    if (journal)
    {
        sd_journal_close(journal);
    }
}

bool JournalReader::open()
{
    int rc = sd_journal_open(
        &journal,
        SD_JOURNAL_LOCAL_ONLY);

    return rc >= 0;
}

bool JournalReader::seektail()
{
    if (!journal)
    {
        return false;
    }

    sd_journal_seek_tail(journal);

    sd_journal_previous(journal);

    return true;
}

std::string JournalReader::getField(
    const char* field)
{
    const void* data = nullptr;
    size_t length = 0;

    int rc = sd_journal_get_data(
        journal,
        field,
        &data,
        &length);

    if (rc < 0)
    {
        return {};
    }

    const char* ptr =
        static_cast<const char*>(data);

    std::string value(ptr, length);

    auto pos = value.find('=');

    if (pos == std::string::npos)
    {
        return {};
    }

    return value.substr(pos + 1);
}

std::optional<JournalEntry>
JournalReader::next()
{
    int rc =
        sd_journal_next(journal);

    if (rc <= 0)
    {
        return std::nullopt;
    }

    JournalEntry entry;

    entry.hostname =
        getField("_HOSTNAME");

    entry.comm =
        getField("_COMM");

    entry.syslogIdentifier =
        getField("SYSLOG_IDENTIFIER");

    entry.message =
        getField("MESSAGE");

    sd_journal_get_realtime_usec(
        journal,
        &entry.realtimeUsec);

    return entry;
}