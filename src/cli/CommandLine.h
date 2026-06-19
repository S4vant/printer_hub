#pragma once

enum class Command
{
    Rebuild,
    Update,
    Send,
    Sync,
    SendZabbix,
    None
};

class CommandLine
{
public:

    static Command parse(
        int argc,
        char* argv[]);
};