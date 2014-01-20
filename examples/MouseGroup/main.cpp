#include <pthread.h>
#include <iostream>
#include "CSCommNet.h"
#include "CSAgent.h"
#include "Mouse.h"

void signal_handler(int sig)
{
    if (sig == SIGINT)
    {
        // do something
    }
    return;
}

int main(void)
{
    signal(SIGINT, signal_handler);

    CSCommNet cccnet(1);
    int mouse_num = 4;
    Mouse *mice[mouse_num];
    CSAgent *agents[mouse_num];
    pthread_t tids[mouse_num];

    // set up each mouse
    for (int i=0; i<mouse_num; i++)
    {
        Mouse *mouse = new Mouse(i);
        CSAgent *agent = new CSAgent(0.8, 0.01);
        agent->InitMemory();

        mouse->ConnectAgent(agent);
        mouse->SetCommNet(&cccnet);
        mice[i] = mouse;
        agents[i] = agent;
        tids[i] = mice[i]->ThreadLaunch();      // launch
    }

    // set up CCCNet
    cccnet.AddMember(1);
    cccnet.AddMember(2);
    cccnet.AddMember(3);
    cccnet.AddMember(4);
    cccnet.AddNeighbour(1, 2);
    cccnet.AddNeighbour(1, 4);
    cccnet.AddNeighbour(2, 1);
    cccnet.AddNeighbour(2, 4);
    cccnet.AddNeighbour(2, 3);
    cccnet.AddNeighbour(3, 2);
    cccnet.AddNeighbour(4, 1);
    cccnet.AddNeighbour(4, 2);

    // wait
    for (int i=0; i<mouse_num; i++)
    {
        pthread_join(tids[i], NULL);

        // all threads quit now
        delete mice[i];
        delete agents[i];
    }

    return 0;
}
