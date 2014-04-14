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
//
// Created on: Oct 19, 2013
//
// -----------------------------------------------------------------------------

#ifndef AVATAR_H_
#define AVATAR_H_
#include <string>
#include "gamcs/Agent.h"

namespace gamcs
{

class DENet;

/**
 * Avatar of an Agent.
 * An Avatar is an agent embodied in flesh.
 */
class Avatar
{
    public:
        Avatar(int id = 0);
        virtual ~Avatar();

        int step();
        void stepLoop(int steps_per_second = -1); /**< step avatar in a continuous loop */
        void connectAgent(Agent *agent); /**< connect to an agent */

    protected:
        int id; /**< avatar's id */
        unsigned long ava_loop_count; /**< loop count */

        Agent *myagent; /**< connected agent */

        virtual Agent::State percieveState() = 0; /**< get current state */
        virtual void performAction(Agent::Action action) = 0; /**< perform an real action */
        virtual OSpace availableActions(Agent::State state) = 0; /**< return a list of all action candidates of a Agent::State */
        virtual float originalPayoff(Agent::State state); /**< original payoff of a state */

    private:
        unsigned long getCurrentTime(); /**< current time in millisecond */
};

/**
 * \brief Connect to an agent.
 * \param agt agent to be connected
 */
inline void Avatar::connectAgent(Agent *agt)
{
    myagent = agt;
}

}    // namespace gamcs
#endif // AVATAR_H_
