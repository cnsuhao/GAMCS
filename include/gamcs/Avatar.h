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
 * Avatar of Agent.
 * An Avatar is an agent embodied in flesh.
 */
class Avatar
{
    public:
        Avatar();
        Avatar(std::string);
        virtual ~Avatar();

        int step();
        void stepLoop(); /**< stepLoop this avatar */

        void connectAgent(Agent *); /**< connect to an agent */
        void setSps(int);

    protected:
        std::string name; /**< avatar's name */
        int sps; /**< number of steps per second */
        unsigned long ava_loop_count; /**< loop count */

        Agent *myagent; /**< connected agent */

        virtual Agent::State percieveState() = 0; /**< get current state */
        virtual void performAction(Agent::Action) = 0; /**< perform an real action */
        virtual OSpace availableActions(Agent::State) = 0; /**< return a list of all action candidates of a Agent::State */
        virtual float originalPayoff(Agent::State); /**< original payoff of a state */

    private:
        unsigned long getCurrentTime(); /**< current time in millisecond */
        unsigned long control_step_time; /**< delta time in millisecond requested bewteen two steps */
};

inline void Avatar::setSps(int s)
{
    sps = s;
    if (sps <= 0)    // no control
        control_step_time = 0;
    else
        control_step_time = 1000 / sps;    // (1 / sps) * 1000
}

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
