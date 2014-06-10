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

Mouse::Mouse() :
        position(0)
{
}

Mouse::~Mouse()
{
}

Agent::State Mouse::perceiveState()
{
    printf("Mouse, State %d\n", position);
    return position;    // just return current position
}

void Mouse::performAction(Agent::Action act)
{
    position += act;

    if (position > 10) position = 10;    // reach the right end
    if (position < -10) position = -10;    // reach the left end
    return;
}

OSpace Mouse::availableActions(Agent::State st)
{
    OSpace acts;

    acts.add(1);    // moving 1 meter right
    acts.add(0);    // staying still
    acts.add(-1);    // moving 1 meter left

    return acts;
}

float Mouse::originalPayoff(Agent::State st)
{
    if (st == 5)
        return 1;    // cheese was put here
    else
        return 0;
}
