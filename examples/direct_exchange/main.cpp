/*
 * main.cpp
 *
 *  Created on: Feb 17, 2014
 *      Author: andy
 */

#include <pthread.h>
#include <stdio.h>
#include "gamcs/CSOSAgent.h"
#include "gamcs/Mysql.h"
#include "Exchanger.h"
#include "ThreadExNet.h"

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("#commnet_test member_num dotfile\n");
        return -1;
    }

    int member_num = atoi(argv[1]);    // number of messager
    ThreadExNet network(1);

    Exchanger *messagers[member_num];
    CSOSAgent *agents[member_num];
    Mysql *mysql[member_num];
    pthread_t tids[member_num];

    // create and set up each messager
    for (int i = 0; i < member_num; i++)
    {
        // storage
        char db_name[16];
        sprintf(db_name, "Member_%d", i);
        Mysql *ml = new Mysql("localhost", "root", "huangk", db_name);

        // agent
        CSOSAgent *agent = new CSOSAgent(i + 1, 0.8, 0.01);    // agent id start from 1
//        agent->LoadMemoryFromStorage(ml);

        // avatar
        Exchanger *messager = new Exchanger(i + 1);   // id starts from 1
        messager->ava_setSps(-1);
        messager->connectMAgent(agent);
        messager->joinExNet(&network);

        mysql[i] = ml;
        messagers[i] = messager;
        agents[i] = agent;
    }

    // load topo
    network.loadTopoFromFile(argv[2]);

    /* launch messagers */
    for (int i = 0; i < member_num; i++)
        tids[i] = messagers[i]->threadExLoop();    // loop in a new thread

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
    network.dumpTopoToFile("dump.exnet");

    printf("*** done!\n");

    return 0;
}

