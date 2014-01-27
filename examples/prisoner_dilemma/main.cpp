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

    float discount_rate = 0.0;  // discount rate determines the final equilibrium!
    CSAgent agentA(discount_rate, 0.01);
    CSAgent agentB(discount_rate, 0.01);
    agentA.SetUnseenActionPayoff(0.0);
    agentB.SetUnseenActionPayoff(0.0);
    agentA.SetStorage(&mysqlA);
    agentB.SetStorage(&mysqlB);

    PrisonerA pA(1);
    PrisonerB pB(2);
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

    return 0;
}



