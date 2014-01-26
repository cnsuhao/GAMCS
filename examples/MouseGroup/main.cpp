#include <pthread.h>
#include "CSThreadCommNet.h"
#include "CSAgent.h"
#include "Mouse.h"
#include "Mysql.h"

int main(void)
{
    CSThreadCommNet commnet(1);
    int mouse_num = 4;
    Mysql *mysql[mouse_num];
    char db_name[16];
    Mouse *mice[mouse_num];
    CSAgent *agents[mouse_num];
    pthread_t tids[mouse_num];

    // create and set up each mouse
    for (int i=0; i<mouse_num; i++)
    {
        // storage
        Mysql *ml = new Mysql();
        sprintf(db_name, "Mouse_%d", i);
        ml->SetDBArgs("localhost", "root", "huangk", db_name);

        // agent
        CSAgent *agent = new CSAgent(0.8, 0.01);
        agent->SetStorage(ml);
        agent->InitMemory();

        // avatar
        Mouse *mouse = new Mouse(i);
        mouse->ConnectAgent(agent);
        mouse->JoinCommNet(&commnet);
        mouse->SetCommFreq(10);

        mysql[i] = ml;
        mice[i] = mouse;
        agents[i] = agent;
    }

    // build neighbours
//    mice[0]->AddNeighbour(2);
//    mice[0]->AddNeighbour(3);
//    mice[1]->AddNeighbour(0);
//    mice[1]->AddNeighbour(3);
//    mice[1]->AddNeighbour(2);
//    mice[2]->AddNeighbour(1);
//    mice[3]->AddNeighbour(0);
//    mice[3]->AddNeighbour(1);
    commnet.LoadTopoFile("nettopo.dat");

    /* launch mice */
    for (int i=0; i<mouse_num; i++)
        tids[i] = mice[i]->ThreadLaunch();      // launch

    // wait
    for (int i=0; i<mouse_num; i++)
    {
        pthread_join(tids[i], NULL);

        // all threads quit now
        delete mice[i];
        delete agents[i];
        delete mysql[i];
    }

    return 0;
}
