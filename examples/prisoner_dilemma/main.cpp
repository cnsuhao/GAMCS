/*
 * main.cpp
 *
 *  Created on: Jan 27, 2014
 *      Author: andy
 */
#include <pthread.h>
#include "CSAgent.h"
#include "Prisoners.h"
#include "Mysql.h"

int main(void)
{
    Mysql mysqlA;
    mysqlA.SetDBArgs("localhost", "root", "huangk",  "PrisonerA");
    Mysql mysqlB;
    mysqlB.SetDBArgs("localhost", "root", "huangk",  "PrisonerB");

    float discount_rate = 0.9;  // discount rate determines the final equilibrium!
    CSAgent agentA(1, discount_rate, 0.01);
    CSAgent agentB(2, discount_rate, 0.01);
    agentA.SetDegreeOfCuriosity(0.0);
    agentB.SetDegreeOfCuriosity(0.0);
    agentA.LoadMemoryFromStorage(&mysqlA);
    agentB.LoadMemoryFromStorage(&mysqlB);

    PrisonerA pA("prisonerA");
    PrisonerB pB("prisonerB");
    pA.ConnectAgent(&agentA);
    pB.ConnectAgent(&agentB);
    pA.SetSps(50);
    pB.SetSps(50);

    // launch
    pthread_t tids[2];
    tids[0] = pA.ThreadLaunch();
    tids[1] = pB.ThreadLaunch();

    // wait
    pthread_join(tids[0], NULL);
    pthread_join(tids[1], NULL);

    agentA.DumpMemoryToStorage(&mysqlA);
    agentB.DumpMemoryToStorage(&mysqlB);
    return 0;
}



