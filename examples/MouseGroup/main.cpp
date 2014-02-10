#include <pthread.h>
#include <stdio.h>
#include "CSThreadCommNet.h"
#include "CSAgent.h"
#include "Mouse.h"
#include "Mysql.h"

int main(void)
{
    int mouse_num = 4;    // number of mouse
    char name[16];  // mouse name

    CSThreadCommNet commnet(1);    // communication network

    // storage of each mouse
    Mysql *mysql[mouse_num];
    char db_name[16];

    Mouse *mice[mouse_num];
    CSAgent *agents[mouse_num];
    pthread_t tids[mouse_num];

    // create and set up each mouse
    for (int i = 0; i < mouse_num; i++)
    {
        // storage
        Mysql *ml = new Mysql();
        sprintf(db_name, "Mouse_%d", i);
        ml->SetDBArgs("localhost", "root", "huangk", db_name);

        // agent
        CSAgent *agent = new CSAgent(i, 0.8, 0.01);
        agent->LoadMemoryFromStorage(ml);

        // avatar
        sprintf(name, "Mouse_%d", i);
        Mouse *mouse = new Mouse(name);
        mouse->ConnectAgent(agent);
        mouse->JoinCommNet(&commnet);

        mysql[i] = ml;
        mice[i] = mouse;
        agents[i] = agent;
    }

    // load topo
    commnet.LoadTopoFromFile("commnet.dot");

    /* launch mice */
    for (int i = 0; i < mouse_num; i++)
        tids[i] = mice[i]->ThreadLaunch();    // launch

    // wait
    for (int i = 0; i < mouse_num; i++)
    {
        pthread_join(tids[i], NULL);

        // all threads quit now
        // save memory to storage
        agents[i]->DumpMemoryToStorage(mysql[i]);

        // clear
        delete mice[i];
        delete agents[i];
        delete mysql[i];
    }
    // save topo structure
    commnet.DumpTopoToFile("commnet.dot");

    return 0;
}
