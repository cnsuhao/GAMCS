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
#include "gimcs/MAgent.h"

namespace gimcs
{

// forward declaration
class Storage;

/**
 * Computer Simulation MAgent, where computer was used to implement an agent.
 */
class CSMAgent: public MAgent
{
    public:
        typedef std::unordered_map<Agent::State, void *> StatesMap; /**< hash map from state value to state  */

        CSMAgent();
        CSMAgent(int);
        CSMAgent(int, float, float);
        ~CSMAgent();

        State_Info_Header *GetStateInfo(State) const; /**< implementing GetStateInfo function */
        void AddStateInfo(const struct State_Info_Header *);
        void UpdateStateInfo(const struct State_Info_Header *);
        void DeleteState(State);
        void UpdatePayoff(State);

        State FirstState() const;
        State NextState() const;
        bool HasState(State) const;

        void LoadMemoryFromStorage(Storage *); /**< load memory from database */
        void DumpMemoryToStorage(Storage *) const; /**< save memory to database */

    private:
        unsigned long state_num; /**< total number of states in memory */
        unsigned long lk_num; /**< total number of links between states in memory */

        struct cs_State *head; /**< memory head*/
        StatesMap states_map; /**< hash map from state values to state  */
        mutable struct cs_State *cur_mst; /**< state  for current state */
        mutable struct cs_State *current_st_index; /**< current state point used by iterator */

        float Prob(const struct cs_EnvAction*, const struct cs_Action *) const; /**< probability of a exact */
        OSpace BestActions(const struct cs_State *, OSpace&) const; /**< find the best action of a state */
        OSpace MaxPayoffRule(Agent::State, OSpace &) const; /**< implementing maximun payoff rule */
        void UpdateStatePayoff(struct cs_State *); /**< update state payoff backward recursively */
        void UpdateMemory(float); /**< implementing UpdateMemory of Agent */

        void PrintProcess(unsigned long, unsigned long, char *) const;

        void LoadState(Storage *, Agent::State); /**< load a state from storage to memory */

        void LinkStates(struct cs_State *, Agent::EnvAction, Agent::Action,
                struct cs_State *); /**< link two states in memory with specfic exact and action */
        void DeleteState(struct cs_State *); /**< delete state from memory network */

        struct cs_State *MallocState(Agent::State); /**< create a new state  in memory */
        struct cs_Action *MallocAct(Agent::Action);
        struct cs_EnvAction *MallocEat(Agent::EnvAction);
        struct cs_BackwardLink *MallocBlk();
        void FreeState(struct cs_State *); /**< free a state  */
        void FreeAct(struct cs_Action *);
        void FreeEat(struct cs_EnvAction *);
        void FreeBlk(struct cs_BackwardLink *);
        void FreeMemory(); /**< free all space of memory in computer memory*/
        void AddStateToCMemory(struct cs_State *); /**< add a state  to computer memory */
        struct cs_State *SearchState(Agent::State) const; /**< search state in memory by its identity */
        void RemoveStateFromCMemory(struct cs_State *);

        struct cs_Action *SearchActInState(Agent::Action,
                const struct cs_State *) const; /**< find the Agent::Action  address according to identity */
        struct cs_EnvAction *SearchEatInAct(Agent::EnvAction,
                struct cs_Action *);
        void AddAct2State(struct cs_Action *, struct cs_State *);
        void AddEat2Act(struct cs_EnvAction *, struct cs_Action *);
        void AddState2Blist(struct cs_State *, struct cs_State *);
        void RemoveActFromState(struct cs_Action *, struct cs_State *);
        void RemoveEatFromAct(struct cs_EnvAction *, struct cs_Action *);
        void RemoveStateFromBList(struct cs_State *, struct cs_State *);
        void BuildStatefromSIHd(const struct State_Info_Header *,
                struct cs_State *);

        float CalStatePayoff(const struct cs_State *) const; /**< calculate payoff of a state */
        float CalActPayoff(Agent::Action, const struct cs_State *) const; /**< calculate payoff of an Agent::Action */
        float _CalActPayoff(const struct cs_Action *) const;
};

/** state information */
struct cs_State
{
        Agent::State st; /**< state value */
        float payoff; /**< state payoff */
        float original_payoff; /**< original payoff of state */
        unsigned long count; /**< state count */
        struct cs_Action *actlist;
        struct cs_BackwardLink *blist; /**< backward links */

        struct cs_State *prev;
        struct cs_State *next;
};

/** implementation of action information */
struct cs_Action
{
        Agent::Action act; /**< action value */
        struct cs_EnvAction *ealist;

        struct cs_Action *next;
};

/** implementation of forward link information */
struct cs_EnvAction
{
        Agent::EnvAction eat;
        unsigned long count; /**< eact count */
        struct cs_State *nstate; /**< next state */

        struct cs_EnvAction *next;
};

/** implementation of backward link information */
struct cs_BackwardLink
{
        struct cs_State *pstate; /**< previous state */
        struct cs_BackwardLink *next;
};

}    // namespace gimcs
#endif // CSMAGENT_H
