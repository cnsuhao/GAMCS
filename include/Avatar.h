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
//
// Created on: Oct 19, 2013
//
// -----------------------------------------------------------------------------

#ifndef AVATAR_H_
#define AVATAR_H_
#include <string>
#include "Agent.h"

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
