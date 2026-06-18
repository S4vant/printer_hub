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
    }

    return 0;
}