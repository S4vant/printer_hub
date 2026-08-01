#pragma once
#include "parser/JournalReader.h"
#include "parser/JobRarser.h"
#include "output/JsonWriter.h"

#include "config/Config.h"
#include "transport/HttpSender.h"
#include "transport/ZabbixSender.h"
#include "data/StateStorage.h"

class Agent
{
public:
    Agent();

    void rebuild();

    void update();

    void send();

    void sync();

    void zabbixsend();

    void zabbixsend_all();

    void zabbixsend_new();

    void zabbixsend_healthcheck();

    void help();

    void exception();
private:
    bool sendToZabbix();
    Config config_;
    StateStorage state_;
    JsonWriter writer_;
    JobParser parser_;
    JournalReader reader_;
};