#pragma once

enum class Command
{
    Rebuild,
    Update,
    Send,
    Sync,
    SendZabbix
};

class CommandLine
{
public:

    static Command parse(
        int argc,
        char* argv[]);
};