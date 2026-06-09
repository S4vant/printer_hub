#include <iostream>
#include <thread>
#include <chrono>

#include "JournalReader.h"

int main()
{
    JournalReader reader;

    if (!reader.open())
    {
        std::cerr
            << "Cannot open journal\n";

        return 1;
    }

    reader.seekTail();

    std::cout
        << "Waiting for CUPS events...\n";

    while (true)
    {
        auto entry = reader.next();

        if (!entry)
        {
            std::this_thread::sleep_for(
                std::chrono::milliseconds(500));

            continue;
        }

        bool isCups =
            entry->comm == "cupsd" ||
            entry->syslogIdentifier == "cupsd";

        if (!isCups)
        {
            continue;
        }

        std::cout << "\n";
        std::cout << "--------------------------------\n";
        std::cout << "HOST    : "
                  << entry->hostname
                  << "\n";

        std::cout << "COMM    : "
                  << entry->comm
                  << "\n";

        std::cout << "SYSLOG  : "
                  << entry->syslogIdentifier
                  << "\n";

        std::cout << "TIME    : "
                  << entry->realtimeUsec
                  << "\n";

        std::cout << "MESSAGE : "
                  << entry->message
                  << "\n";
    }
}