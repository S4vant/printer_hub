#pragma once

enum class Command
{
    Rebuild,
    Update,
    Send,
    Sync,
    SendZabbixAll,
    SendZabbixNew,
    None
};

class CommandLine
{
public:

    static Command parse(
        int argc,
        char* argv[]);
};