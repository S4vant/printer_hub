#include "CommandLine.h"

#include <string>

Command CommandLine::parse(
    int argc,
    char* argv[])
{
    if (argc < 2)
    {
        return Command::Sync;
    }

    std::string command =
        argv[1];

    if (command == "rebuild")
    {
        return Command::Rebuild;
    }

    if (command == "update")
    {
        return Command::Update;
    }

    if (command == "send")
    {
        return Command::Send;
    }

    if (command == "sync")
    {
        return Command::Sync;
    }

    if (command == "zabbixsend_all")
    {
        return Command::SendZabbixAll;
    }
    if (command == "zabbixsend_new")
    {
        return Command::SendZabbixNew;
    }
    if (command == "help")
    {
        return Command::Help;
    }

    return Command::None;
}