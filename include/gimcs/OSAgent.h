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
//
// Created on: Feb 17, 2014
//
// -----------------------------------------------------------------------------

#ifndef OSAGENT_H_
#define OSAGENT_H_

#include "gimcs/Agent.h"
#include "gimcs/Storage.h"

namespace gimcs
{

/**
 *  Open Storage Agent.
 *  Agent with storage interface.
 */
class OSAgent: public Agent, public Storage
{
    public:
        OSAgent()
        {
        }

        OSAgent(int i) :
                Agent(i)
        {
        }

        OSAgent(int i, float dr, float th) :
                Agent(i, dr, th)
        {
        }

        virtual ~OSAgent()
        {
        }

        virtual void updatePayoff(State) = 0; /**< update payoff beginning from a specified state */
};

}    // namespace gimcs

#endif /* OSAGENT_H_ */
