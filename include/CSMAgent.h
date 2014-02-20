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


#ifndef CSMAGENT_H
#define CSMAGENT_H
#include <unordered_map>
#include "MAgent.h"

namespace gimcs
{

class Storage;

/**
 * Computer Simulation MAgent, where computer was used to implement an agent.
 */
class CSMAgent: public MAgent
{
    public:
        typedef std::unordered_map<Agent::State, void *> StatesMap; /**< hash map from state value to state struct */
        enum SgFlag
        {
            SAVED, NEW, MODIFIED
        }; /**< storage status of a state */

        CSMAgent();
        CSMAgent(int);
        CSMAgent(int, float, float);
        ~CSMAgent();

        struct State_Info_Header *GetStateInfo(State) const; /**< implementing GetStateInfo function */
        void AddStateInfo(const struct State_Info_Header *);
        void UpdateStateInfo(const struct State_Info_Header *);
        void DeleteState(State);
        void UpdateState(State);

        State FirstState() const;
        State NextState() const;
        bool HasState(State) const;

        void LoadMemoryFromStorage(Storage *); /**< load memory from database */
        void DumpMemoryToStorage(Storage *) const; /**< save memory to database */

    private:
        unsigned long state_num; /**< total number of states in memory */
        unsigned long lk_num; /**< total number of links between states in memory */

        OSpace MaxPayoffRule(Agent::State, OSpace &) const; /**< implementing maximun payoff rule */
        void UpdateMemory(float); /**< implementing UpdateMemory of Agent */

        void PrintProcess(unsigned long, unsigned long, char *) const;

        struct cs_State *head; /**< memory head*/
        StatesMap states_map; /**< hash map from state values to state struct */
        mutable struct cs_State *cur_mst; /**< state struct for current state */
        mutable struct cs_State *current_st_index; /**< current state point used by iterator */

        void LoadState(Storage *, Agent::State); /**< load a state from storage to memory */

        void FreeMemory(); /**< free all space of memory in computer memory*/

        void DeleteState(struct cs_State *); /**< remove "root" state */

        void LinkStates(struct cs_State *, Agent::EnvAction, Agent::Action,
                struct cs_State *); /**< link two states in memory with specfic exact and action */
        OSpace BestActions(const struct cs_State *, OSpace&) const; /**< find the best action of a state */
        struct cs_State *SearchState(Agent::State) const; /**< search state in memory by its identity */
        void UpdateState(struct cs_State *); /**< update state payoff backward recursively */

        struct cs_State *NewState(Agent::State); /**< create a new state struct in memory */
        struct cs_EnvAction *NewEa(Agent::EnvAction);
        struct cs_Action *NewAc(Agent::Action);
        struct cs_ForwardArcState *NewFas(Agent::EnvAction, Agent::Action);
        struct cs_BackArcState *NewBas();
        void FreeState(struct cs_State *); /**< free a state struct */
        void FreeEa(struct cs_EnvAction *);
        void FreeAc(struct cs_Action *);
        void FreeFas(struct cs_ForwardArcState *);
        void FreeBas(struct cs_BackArcState *);
        void AddStateToMemory(struct cs_State *); /**< add a state struct to memory */

        struct cs_Action *Act2Struct(Agent::Action,
                const struct cs_State *) const; /**< find the Agent::Action struct address according to identity */
        struct cs_EnvAction *Eat2Struct(Agent::EnvAction,
                const struct cs_State *) const; /**< find the exact strut address according to identity */
        struct cs_State *StateByEatAct(Agent::EnvAction, Agent::Action,
                const struct cs_State *) const; /**< find the following state according to exact and Agent::Action*/
        float MaxPayoffInEat(Agent::EnvAction, const struct cs_State *) const; /**< maximun payoff of all following states under a specfic exact */
        float Prob(const struct cs_EnvAction*, const struct cs_State *) const; /**< probability of a exact */

        float CalStatePayoff(const struct cs_State *) const; /**< calculate payoff of a state */
        float CalActPayoff(Agent::Action, const struct cs_State *) const; /**< calculate payoff of an Agent::Action */
};

/** implementation of environment action information */
struct cs_EnvAction
{
        Agent::EnvAction eat; /**< eact value */
        unsigned long count; /**< eact count */
        struct cs_EnvAction *next; /**< next struct */
};

/** implementation of action information */
struct cs_Action
{
        Agent::Action act; /**< action value */
        float payoff; /**< action payoff */
        struct cs_Action *next;
};

/** implementation of forward link information */
struct cs_ForwardArcState
{
        Agent::EnvAction eat; /**< exact */
        Agent::Action act; /**< action */
        struct cs_State *nstate; /**< following state */
        struct cs_ForwardArcState *next;
};

/** implementation of backward link information */
struct cs_BackArcState
{
        struct cs_State *pstate; /**< previous state */
        struct cs_BackArcState *next;
};

/** state information */
struct cs_State
{
        Agent::State st; /**< state value */
        float payoff; /**< state payoff */
        float original_payoff; /**< original payoff of state */
        unsigned long count; /**< state count */
        enum CSMAgent::SgFlag flag; /**< flag used for storage */
        struct cs_EnvAction *ealist; /**< exacts of this state */
        struct cs_Action *atlist; /**< actions of this state */
        struct cs_ForwardArcState *flist; /**< forward links */
        struct cs_BackArcState *blist; /**< backward links */
        struct cs_State *prev;
        struct cs_State *next;
};

}    // namespace gimcs
#endif // CSMAGENT_H
