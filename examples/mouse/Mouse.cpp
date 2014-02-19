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


#include "Mouse.h"

Mouse::Mouse(std::string n) :
        Avatar(n), count(0)
{
    position = 3;
}

Mouse::~Mouse()
{
    //dtor
}

Agent::State Mouse::GetCurrentState()
{
    printf("%s, State %ld\n", name.c_str(), position);
    return position;
}

void Mouse::PerformAction(Agent::Action act)
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

OSpace Mouse::ActionCandidates(Agent::State st)
{
//    UNUSED(st);
    if (count < 500)
    {
        OSpace acts;
        acts.Clear();
        acts.Add(1);
        acts.Add(-1);
        count++;
        return acts;
    }
    else
        return OSpace();    // return an empty list
}

float Mouse::OriginalPayoff(Agent::State st)
{
    if (st == 6)
        return 1;
    else
        return 0;
}
