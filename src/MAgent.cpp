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


#include "MAgent.h"

namespace gimcs
{

MAgent::MAgent()
{
}

MAgent::MAgent(int i) :
        Agent(i)
{
}

MAgent::MAgent(int i, float dr, float th) :
        Agent(i, dr, th)
{
}

MAgent::~MAgent()
{
}

}    // namespace gimcs
