#pragma once

class Agent
{
public:

    void rebuild();

    void update();

    void send();

    void sync();

    void zabbixsend_all();

    void zabbixsend_new();
};