/*
 * main.cpp
 *
 *  Created on: Feb 17, 2014
 *      Author: andy
 */

#ifdef _WIN32_
#include <windows.h>
#else
#include <pthread.h>
#endif
#include <stdio.h>
#include "gamcs/CSOSAgent.h"
#include "gamcs/Mysql.h"
#include "Exchanger.h"
#include "ThreadExNet.h"

#ifdef _WIN32_
DWORD WINAPI MyThreadFunction(LPVOID exer)
{
    Exchanger *exchanger = (Exchanger *) exer;
    unsigned long count = 0;
    int ret;
    while (count < 1000)
    {
        ret = exchanger->exStep();
        if (ret == -1) break;

        ++count;
    }

    return 0;
}

#else
void *thread_fun(void *exer)
{
    Exchanger *exchanger = (Exchanger *) exer;
    unsigned long count = 0;
    int ret;
    while (count < 1000)
    {
        ret = exchanger->exStep();
        if (ret == -1) break;

        ++count;
    }

    return NULL;
}
#endif

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("#commnet_test member_num dotfile\n");
        return -1;
    }

    int member_num = atoi(argv[1]);    // number of messager
    ThreadExNet network(1);

    Exchanger **messagers = (Exchanger **) malloc(
            member_num * sizeof(Exchanger *));
    CSOSAgent **agents = (CSOSAgent **) malloc(
            member_num * sizeof(CSOSAgent *));
    Mysql **mysql = (Mysql **) malloc(member_num * sizeof(Mysql *));

#ifdef _WIN32_
    HANDLE hThreadArray[member_num];
#else
    pthread_t tids[member_num];
#endif

    // create and set up each messager
    for (int i = 0; i < member_num; i++)
    {
        // storage
        char db_name[16];
        sprintf(db_name, "Member_%d", i);
        Mysql *ml = new Mysql("localhost", "root", "huangk", db_name);

        // agent
        CSOSAgent *agent = new CSOSAgent(i + 1, 0.8, 0.01);    // agent id start from 1
        // agent->LoadMemoryFromStorage(ml);

        // avatar
        Exchanger *messager = new Exchanger(i + 1);    // id starts from 1
        messager->connectOSAgent(agent);
        messager->joinExNet(&network);

        mysql[i] = ml;
        messagers[i] = messager;
        agents[i] = agent;
    }

    // load topo
    network.loadTopoFromFile(argv[2]);

    /* launch messagers */
    for (int i = 0; i < member_num; i++)
    {
#ifdef _WIN32_
        hThreadArray[i] = CreateThread(NULL, 0, MyThreadFunction, messagers[i], 0, NULL);
#else
        pthread_create(&tids[i], NULL, thread_fun, messagers[i]);
#endif
    }

    // wait for all threads to terminate
#ifdef _WIN32_
    WaitForMultipleObjects(member_num, hThreadArray, TRUE, INFINITE);

#else
    for (int i = 0; i < member_num; i++)
    {
        pthread_join(tids[i], NULL);
    }
#endif

    for (int i = 0; i < member_num; i++)
    {
        // all threads quit now
        // save memory to storage
        // agents[i]->DumpMemoryToStorage(mysql[i]);

        // clear
#ifdef _WIN32_
        CloseHandle(hThreadArray[i]);
#endif

        delete messagers[i];
        delete agents[i];
        delete mysql[i];
    }
    // save topo structure
    network.dumpTopoToFile("dump.exnet");

    free(messagers);
    free(agents);
    free(mysql);
    printf("*** done!\n");

    return 0;
}

