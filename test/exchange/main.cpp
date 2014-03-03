/*
 * main.cpp
 *
 *  Created on: Feb 17, 2014
 *      Author: andy
 */

#include <pthread.h>
#include <stdio.h>
#include "gimcs/CSThreadExNet.h"
#include "gimcs/CSMAgent.h"
#include "gimcs/Mysql.h"
#include "messager.h"

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("#commnet_test member_num dotfile\n");
        return -1;
    }

    int member_num = atoi(argv[1]);    // number of messager
    CSThreadExNet network(1);

    Messager *messagers[member_num];
    CSMAgent *agents[member_num];
    Mysql *mysql[member_num];
    pthread_t tids[member_num];

    // create and set up each messager
    for (int i = 0; i < member_num; i++)
    {
        // storage
        char db_name[16];
        Mysql *ml = new Mysql();
        sprintf(db_name, "Member_%d", i);
        ml->SetDBArgs("localhost", "root", "huangk", db_name);

        // agent
        CSMAgent *agent = new CSMAgent(i + 1, 0.8, 0.01);    // agent id start from 1
//        agent->LoadMemoryFromStorage(ml);

        // avatar
        Messager *messager = new Messager(i + 1);
        messager->Ava_SetSps(-1);
        messager->ConnectMAgent(agent);
        messager->JoinExNet(&network);

        mysql[i] = ml;
        messagers[i] = messager;
        agents[i] = agent;
    }

    // load topo
    network.LoadTopoFromFile(argv[2]);

    /* launch messagers */
    for (int i = 0; i < member_num; i++)
        tids[i] = messagers[i]->ThreadRun();    // launch

    // wait
    for (int i = 0; i < member_num; i++)
    {
        pthread_join(tids[i], NULL);

        // all threads quit now
        // save memory to storage
//        agents[i]->DumpMemoryToStorage(mysql[i]);

        // clear
        delete messagers[i];
        delete agents[i];
        delete mysql[i];
    }
    // save topo structure

    printf("------- quit!\n");

    return 0;
}

