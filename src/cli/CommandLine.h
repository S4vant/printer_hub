#pragma once

enum class Command
{
    Rebuild,
    Update,
    Send,
    Sync,
    SendZabbixAll,
    SendZabbixNew,
    help,
    None
};

class CommandLine
{
public:

    static Command parse(
        int argc,
        char* argv[]);
};