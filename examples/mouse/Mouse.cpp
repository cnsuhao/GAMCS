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

Mouse::Mouse() : position(3)
{
}

Mouse::~Mouse()
{
    //dtor
}

Agent::State Mouse::percieveState()
{
    printf("Mouse, State %d\n", position);
    return position;
}

void Mouse::performAction(Agent::Action act)
{
    position += act;

    if (position > 8) position = 8;
    if (position < 1) position = 1;
    return;
}

/** \brief Get all outputs of each possible input.
 * By default, for a "I:N/O:M" it will return outputs with values from 1 to M for each input.
 *
 * \param in input identity
 * \return all possible outputs for the input
 *
 */

OSpace Mouse::availableActions(Agent::State st)
{
//    UNUSED(st);
    OSpace acts;
    acts.clear();
    acts.add(1);
    acts.add(-1);

    return acts;
}

float Mouse::originalPayoff(Agent::State st)
{
    if (st == 6)
        return 1;
    else
        return 0;
}
