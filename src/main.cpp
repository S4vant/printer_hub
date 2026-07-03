#include "agent/Agent.h"
#include "cli/CommandLine.h"

int main(
    int argc,
    char* argv[])
{
    Agent agent;

    switch (
        CommandLine::parse(
            argc,
            argv))
    {
        case Command::Rebuild:
            agent.rebuild();
            break;

        case Command::Update:
            agent.update();
            break;

        case Command::Send:
            agent.send();
            break;

        case Command::Sync:
            agent.sync();
            break;

        case Command::SendZabbixNew:
            agent.zabbixsend_new();
            break;

        case Command::SendZabbixAll:
            agent.zabbixsend_all();
            break;
        case Command::help:
            agent.help();
            break;

        case Command::None:
            agent.ecxeption();
            break;
    }

    return 0;
}