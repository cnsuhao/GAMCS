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

#ifndef AGENT_H
#define AGENT_H
#include "gamcs/TSGIOM.h"
#include "gamcs/debug.h"

namespace gamcs
{

#define ST_FMT IN_FMT		/**< State format used in printf */
#define ACT_FMT OUT_FMT		/**< Action format used in printf */

/**
 * Intelligent Agent
 */
class Agent: public TSGIOM
{
    public:
        typedef GIOM::Input State; /**< for an agent we call an input a state */
        typedef GIOM::Output Action; /**< for an agent we call an action an output */
        typedef GIOM::Output EnvAction; /**< environment action */

        enum Mode
        {
            ONLINE = 0, /**< in this mode, an agent will learn as it does, and uses what it has learned */
            EXPLORE /**< in this mode, an agent will learn as it does, but not use what it has learned, instead it will act randomly */
        };

        Agent(int id = 0, float discount_rate = 0.9, float threshold = 0.01);
        virtual ~Agent();

        void setMode(Mode mode); /**< set the learning mode of agent */
        void update(float original_payoff); /**< update memory, this function will call updateMemory() to do the real update */

        static const State INVALID_STATE;
        static const Action INVALID_ACTION;
        static const float INVALID_PAYOFF; /**< use the maximum value to represent the invalid payoff */

    protected:
        int id; /**< agent Id */
        float discount_rate; /**< discount rate (0<,<1) when calculate state payoff */
        float threshold; /**< threshold used in payoff updating */
        Mode learning_mode; /**< learning mode, ONLINE by default */

        OSpace constrain(State state, OSpace &avaliable_actions) const; /**< reimplement restrict using maximum payoff rule  */

        /** These two functions are implementation dependent, declared as pure virtual functions */
        virtual OSpace maxPayoffRule(State state,
                OSpace &available_actions) const = 0; /**< implementation of maximum payoff rule */
        virtual void updateMemory(float original_payoff) = 0; /**<  update states in memory given current state's original payoff*/
};

/** Action information header */
struct Action_Info_Header
{
        Agent::Action act; /**< action value */
        unsigned long eat_num; /**< number of environment actions which have been observed under this action */
};

/** Environment Action information header */
struct EnvAction_Info
{
        Agent::EnvAction eat; /**< environment action value */
        unsigned long count; /**< count of experiencing times */
        Agent::State nst; /**< the following state value */
};

/** Header of state information, this structure can be used to share with other agents */
struct State_Info_Header
{
        Agent::State st; /**< state value */
        float original_payoff; /**< original payoff */
        float payoff; /**< payoff */
        unsigned long count; /**< times of traveling through this state */
        unsigned long act_num; /**< number of actions which have been performed */
        unsigned int size; /**< size of the header (in Byte) */
};

/** memory information */
struct Memory_Info
{
        float discount_rate; /**< discount rate */
        float threshold; /**< threshold */
        unsigned long state_num; /**< total number of states in memory */
        unsigned long lk_num; /**< total number of links between states in memory */
        Agent::State last_st; /**< last experienced state when saving memory */
        Agent::Action last_act; /**< last performed action when saving memory */
};

}    // namespace gamcs
#endif // AGENT_H
