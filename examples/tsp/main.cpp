/*
 * main.cpp
 *
 *  Created on: Feb 12, 2014
 *      Author: andy
 */

#include <pthread.h>
#include <stdio.h>
#include "Saleman.h"
#include "CSMMIAgent.h"
#include "Mysql.h"

int main(void)
{
    CSMMIAgent *agent = new CSMMIAgent(1, 0.8, 0.01);
    agent->SetDegreeOfCuriosity(-5000);

    Saleman *saleman = new Saleman("Saleman");
    saleman->SetSps(-1);
    saleman->ConnectAgent(agent);

    pthread_t tid = saleman->ThreadLaunch();

    pthread_join(tid, NULL);

    delete saleman;
    delete agent;
}

