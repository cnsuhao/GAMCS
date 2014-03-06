// -----------------------------------------------------------------------------
//
// GIMCS -- Generalized Intelligence Model and Computer Simulation
//
// Copyright (C) 2013-2014, Andy Huang  <andyspider@126.com>
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// -----------------------------------------------------------------------------

#include <pthread.h>
#include "gimcs/CSMAgent.h"
#include "Prisoners.h"
#ifdef _WITH_MYSQL_
#include "gimcs/Mysql.h"
#endif

int main(void)
{
    float discount_rate = 0.0;    // discount rate determines the final equilibrium!
    CSMAgent agentA(1, discount_rate, 0.01);
    CSMAgent agentB(2, discount_rate, 0.01);

#ifdef _WITH_MYSQL_
    Mysql mysqlA;
    mysqlA.SetDBArgs("localhost", "root", "huangk", "PrisonerA");
    Mysql mysqlB;
    mysqlB.SetDBArgs("localhost", "root", "huangk", "PrisonerB");

    agentA.LoadMemoryFromStorage(&mysqlA);
    agentB.LoadMemoryFromStorage(&mysqlB);
#endif

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

#ifdef _WITH_MYSQL_
    agentA.DumpMemoryToStorage(&mysqlA);
    agentB.DumpMemoryToStorage(&mysqlB);
#endif
    return 0;
}

