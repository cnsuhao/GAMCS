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
#include <stdio.h>
#include "Saleman.h"
#include "CSMAgent.h"
#include "Mysql.h"

int main(void)
{
    CSMAgent *agent = new CSMAgent(1, 0.8, 0.01);
    agent->SetDegreeOfCuriosity(-5000);

    Saleman *saleman = new Saleman("Saleman");
    saleman->SetSps(-1);
    saleman->ConnectAgent(agent);

    pthread_t tid = saleman->ThreadLaunch();

    pthread_join(tid, NULL);

    delete saleman;
    delete agent;
}

