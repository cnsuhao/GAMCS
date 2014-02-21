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
// Created on: Oct 19, 2013
//
// -----------------------------------------------------------------------------

#ifndef AVATAR_H_
#define AVATAR_H_
#include <string>
#include "gimcs/Agent.h"

namespace gimcs
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

        void Launch(); /**< launch this avatar */

        void ConnectAgent(Agent *); /**< connect to an agent */
        void SetSps(int);

    protected:
        std::string name; /**< avatar's name */
        int sps; /**< number of steps per second */
        unsigned long ava_loop_count; /**< loop count */

        Agent *myagent; /**< connected agent */

        virtual Agent::State GetCurrentState() = 0; /**< get current state */
        virtual void PerformAction(Agent::Action) = 0; /**< perform an real action */
        virtual OSpace ActionCandidates(Agent::State) = 0; /**< return a list of all action candidates of a Agent::State */
        virtual float OriginalPayoff(Agent::State); /**< original payoff of a state */

    private:
        unsigned long GetCurrentTime(); /**< current time in millisecond */
        unsigned long control_step_time; /**< delta time in millisecond requested bewteen two steps */
};

inline void Avatar::SetSps(int s)
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
inline void Avatar::ConnectAgent(Agent *agt)
{
    myagent = agt;
}

}    // namespace gimcs
#endif // AVATAR_H_
