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
#include "CSMAgent.h"
#include "Mysql.h"

int main(void)
{
    Mysql mysql;
    mysql.SetDBArgs("localhost", "root", "huangk", "Mouse");
    CSMAgent ma(1, 0.9, 0.01);
    ma.LoadMemoryFromStorage(&mysql);

    Mouse mouse("Mouse");
    mouse.ConnectAgent(&ma);
    mouse.Launch();

    ma.DumpMemoryToStorage(&mysql);
}
