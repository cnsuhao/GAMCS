// -----------------------------------------------------------------------------
//
// GAMCS -- Generalized Agent Model and Computer Simulation
//
// Copyright (C) 2013-2014, Andy Huang  <andyspider@126.com>
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// -----------------------------------------------------------------------------

#include "Mouse.h"
#include "gamcs/CSOSAgent.h"
#ifdef _MYSQL_FOUND_
//#include "gamcs/Mysql.h"
#endif
#include "gamcs/DotViewer.h"

int main(void)
{
    CSOSAgent agent(1, 0.9, 0.01);

#ifdef _MYSQL_FOUND_
//    Mysql mysql("localhost", "root", "huangk", "Mouse");
//    agent.loadMemoryFromStorage(&mysql);
#endif

    Mouse mouse;
    mouse.connectAgent(&agent);

    // run 500 times
    int count = 0;
    while (count < 500)
    {
        mouse.step();
        count++;
    }

//    DotViewer dv;
//    dv.attachStorage(&agent);
//    dv.show();
    printf("New mouse created\n");
    // create a new mouse with the memory
    Mouse new_mouse;
    new_mouse.connectAgent(&agent);

    count = 0;
    while (count < 500)
    {
        new_mouse.step();
        count++;
    }

#ifdef _MYSQL_FOUND_
//    agent.dumpMemoryToStorage(&mysql);
#endif

    return 0;
}
