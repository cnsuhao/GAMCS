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

#include "Mouse.h"
#include "gamcs/CSOSAgent.h"
#ifdef _WITH_MYSQL_
#include "gamcs/Mysql.h"
#endif

int main(void)
{
    CSOSAgent ma(1, 0.9, 0.01);
#ifdef _WITH_MYSQL_
    Mysql mysql;
    mysql.setDBArgs("localhost", "root", "huangk", "Mouse");
    ma.loadMemoryFromStorage(&mysql);
#endif

    Mouse mouse("Mouse");
    mouse.connectAgent(&ma);
    mouse.launch();

#ifdef _WITH_MYSQL_
    ma.dumpMemoryToStorage(&mysql);
#endif
}
