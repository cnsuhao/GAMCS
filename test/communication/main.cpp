/*
 * main.cpp
 *
 *  Created on: Feb 11, 2014
 *      Author: andy
 */

#include <pthread.h>
#include <stdio.h>
#include "CSThreadCommNet.h"
#include "CSAgent.h"
#include "member.h"
#include "Mysql.h"

int main(void)
{
    int member_num = 4;    // number of member
    char name[16];  // member name

    CSThreadCommNet commnet(1);    // communication network

    // storage of each member
//    Mysql *mysql[member_num];
//    char db_name[16];

    Member *members[member_num];
    CSAgent *agents[member_num];
    pthread_t tids[member_num];

    // create and set up each member
    for (int i = 0; i < member_num; i++)
    {
        // storage
//        Mysql *ml = new Mysql();
//        sprintf(db_name, "Member_%d", i);
//        ml->SetDBArgs("localhost", "root", "huangk", db_name);

        // agent
        CSAgent *agent = new CSAgent(i, 0.8, 0.01);
//        agent->LoadMemoryFromStorage(ml);

        // avatar
        sprintf(name, "Member_%d", i);
        Member *member = new Member(name);
        member->ConnectAgent(agent);
        member->JoinCommNet(&commnet);

//        mysql[i] = ml;
        members[i] = member;
        agents[i] = agent;
    }

    // load topo
    commnet.LoadTopoFromFile("commnet.dot");

    /* launch members */
    for (int i = 0; i < member_num; i++)
        tids[i] = members[i]->ThreadLaunch();    // launch

    // wait
    for (int i = 0; i < member_num; i++)
    {
        pthread_join(tids[i], NULL);

        // all threads quit now
        // save memory to storage
//        agents[i]->DumpMemoryToStorage(mysql[i]);

        // clear
        delete members[i];
        delete agents[i];
//        delete mysql[i];
    }
    // save topo structure
    commnet.DumpTopoToFile("commnet.dot");

    return 0;
}


