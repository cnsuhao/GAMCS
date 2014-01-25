#include <pthread.h>
#include "CSThreadCommNet.h"
#include "CSAgent.h"
#include "Mouse.h"
#include "Mysql.h"

int main(void)
{
    CSThreadCommNet cccnet(1);
    int mouse_num = 4;
    Mysql *mysql[mouse_num];
    char db_name[16];
    Mouse *mice[mouse_num];
    CSAgent *agents[mouse_num];
    pthread_t tids[mouse_num];

    // set up CCCNet
    cccnet.AddMember(0);
    cccnet.AddMember(1);
    cccnet.AddMember(2);
    cccnet.AddMember(3);
//    cccnet.AddNeighbour(0, 1);
//    cccnet.AddNeighbour(1, 0);
//
    cccnet.AddNeighbour(0, 2);
    cccnet.AddNeighbour(0, 3);
    cccnet.AddNeighbour(1, 0);
    cccnet.AddNeighbour(1, 3);
    cccnet.AddNeighbour(1, 2);
    cccnet.AddNeighbour(2, 1);
    cccnet.AddNeighbour(3, 0);
    cccnet.AddNeighbour(3, 1);

    // set up each mouse
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
        mouse->SetCommNet(&cccnet);
        mouse->SetCommFreq(10);

        mysql[i] = ml;
        mice[i] = mouse;
        agents[i] = agent;
        tids[i] = mice[i]->ThreadLaunch();      // launch
    }

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
