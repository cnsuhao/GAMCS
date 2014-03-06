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

#include "Mouse.h"
#include "gimcs/CSMAgent.h"
#ifdef _WITH_MYSQL_
#include "gimcs/Mysql.h"
#endif

int main(void)
{
    CSMAgent ma(1, 0.9, 0.01);
#ifdef _WITH_MYSQL_
    Mysql mysql;
    mysql.SetDBArgs("localhost", "root", "huangk", "Mouse");
    ma.LoadMemoryFromStorage(&mysql);
#endif

    Mouse mouse("Mouse");
    mouse.ConnectAgent(&ma);
    mouse.Launch();

#ifdef _WITH_MYSQL_
    ma.DumpMemoryToStorage(&mysql);
#endif
}
