#pragma once

#include <systemd/sd-journal.h>
#include <optional>

#include "JournalEntry.h"

class JournalReader {
public:
    JournalReader()
    ~JournalReader()

    bool open();

    bool seektail();

    std::optional<JournalEntry> next();

private:
    srd::string getField(const char* field);
private:
    sd_journal* journal = nullptr;

};

