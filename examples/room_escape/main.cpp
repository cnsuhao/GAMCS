// -----------------------------------------------------------------------------
//
// GAMCS -- Generalized Intelligence Model and Computer Simulation
//
// Copyright (C) 2013-2014, Andy Huang  <andyspider@126.com>
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// -----------------------------------------------------------------------------

#include <stdio.h>
#include "gamcs/CSOSAgent.h"
#ifdef _WITH_MYSQL_
#include "gamcs/Mysql.h"
#endif
#include "Escapee.h"
#include "gamcs/DotViewer.h"

int main(void)
{
    CSOSAgent agent(1, 0.9, 0.01);

    Escapee escapee(2);
    escapee.connectAgent(&agent);

    int count = 0;
    while (count < 200)
    {
        escapee.step();

        count++;
    }

    // show states
    DotViewer dv;
    dv.attachStorage(&agent);
    dv.show();

    printf("spawn a new escapee with the learned agent.\n");
    Escapee escapee2(1);
    escapee2.setSps(2);
    escapee2.connectAgent(&agent);

    count = 0;
    while (count < 10)
    {
        escapee2.step();

        count++;
    }

    return 0;
}

