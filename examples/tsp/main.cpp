/*
 * main.cpp
 *
 *  Created on: Feb 12, 2014
 *      Author: andy
 */

#include <pthread.h>
#include <stdio.h>
#include "CSThreadCommNet.h"
#include "Saleman.h"
#include "CSAgent.h"
#include "Mysql.h"

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("#./tsp num_saleman topofile\n");
        return -1;
    }

    int num_saleman = atoi(argv[1]);
    char *topofile = argv[2];
    char name[16];

    CSThreadCommNet commnet(1);

    Saleman *salemen[num_saleman];
    CSAgent *agents[num_saleman];
    pthread_t tids[num_saleman];

    for (int i = 0; i < num_saleman; i++)
    {
        CSAgent *agent = new CSAgent(i + 1, 0.8, 0.01);
        agent->SetDegreeOfCuriosity(-10000);

        sprintf(name, "Saleman_%d", i + 1);
        Saleman *saleman = new Saleman(name);
        saleman->SetSps(-1);
        saleman->ConnectAgent(agent);
        saleman->JoinCommNet(&commnet);

        salemen[i] = saleman;
        agents[i] = agent;
    }

    commnet.LoadTopoFromFile(topofile);

    for (int i = 0; i < num_saleman; i++)
        tids[i] = salemen[i]->ThreadLaunch();

    for (int i=0; i<num_saleman; i++)
    {
        pthread_join(tids[i], NULL);

        delete salemen[i];
        delete agents[i];
    }

    printf("******** quit! ********\n");
}

