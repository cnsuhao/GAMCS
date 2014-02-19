// -----------------------------------------------------------------------------
//
// GIMCS -- Generalized Intelligence Model and Computer Simulation
//
// Copyright (C) 2013-2014, Andy Huang  <andyspider@126.com>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//
// -----------------------------------------------------------------------------


#include <pthread.h>
#include "CSMAgent.h"
#include "Prisoners.h"
#include "Mysql.h"

int main(void)
{
    Mysql mysqlA;
    mysqlA.SetDBArgs("localhost", "root", "huangk", "PrisonerA");
    Mysql mysqlB;
    mysqlB.SetDBArgs("localhost", "root", "huangk", "PrisonerB");

    float discount_rate = 0.9;    // discount rate determines the final equilibrium!
    CSMAgent agentA(1, discount_rate, 0.01);
    CSMAgent agentB(2, discount_rate, 0.01);
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

