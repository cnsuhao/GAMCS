#ifndef CSIAGENT_H
#define CSIAGENT_H
#include <unordered_map>
#include "IAgent.h"

class Storage;

/**
 * Computer Simulation of Agent, which uses computers to implement an agent.
 */
class CSIAgent: public IAgent
{
    public:
        typedef std::unordered_map<IAgent::State, void *> StatesMap; /**< hash map from state value to state struct */
        enum SgFlag
        {
            SAVED, NEW, MODIFIED
        }; /**< storage status of a state */

        CSIAgent();
        CSIAgent(int);
        CSIAgent(int, float, float);
        ~CSIAgent();

        void LoadMemoryFromStorage(Storage *); /**< load memory from database */
        void DumpMemoryToStorage(Storage *); /**< save memory to database */

    private:
        unsigned long state_num; /**< total number of states in memory */
        unsigned long lk_num; /**< total number of links between states in memory */

        OSpace MaxPayoffRule(IAgent::State, OSpace &); /**< implementing maximun payoff rule */
        void UpdateMemory(float); /**< implementing UpdateMemory of Agent */
        struct State_Info_Header *GetStateInfo(IAgent::State) const; /**< implementing GetStateInfo function */

        void PrintProcess(unsigned long, unsigned long, char *) const;

        struct cs_State *head; /**< memory head*/
        StatesMap states_map; /**< hash map from state values to state struct */
        struct cs_State *cur_mst; /**< state struct for current state value */

        void LoadState(Storage *, IAgent::State); /**< load a state from storage to memory */

        void FreeMemory(); /**< free all space of memory in computer memory*/

        void RemoveState(struct cs_State *); /**< remove "root" state */

        void LinkStates(struct cs_State *, IAgent::EnvAction, IAgent::Action,
                struct cs_State *); /**< link two states in memory with specfic exact and action */
        OSpace BestActions(const struct cs_State *, OSpace&); /**< find the best action of a state */
        struct cs_State *SearchState(IAgent::State) const; /**< search state in memory by its identity */
        void UpdateState(struct cs_State *); /**< update state payoff backward recursively */

        struct cs_State *NewState(IAgent::State); /**< create a new state struct in memory */
        struct cs_EnvAction *NewEa(IAgent::EnvAction);
        struct cs_Action *NewAc(IAgent::Action);
        struct cs_ForwardArcState *NewFas(IAgent::EnvAction, IAgent::Action);
        struct cs_BackArcState *NewBas();
        void FreeState(struct cs_State *); /**< free a state struct */
        void FreeEa(struct cs_EnvAction *);
        void FreeAc(struct cs_Action *);
        void FreeFas(struct cs_ForwardArcState *);
        void FreeBas(struct cs_BackArcState *);
        void AddStateToMemory(struct cs_State *); /**< add a state struct to memory */

        struct cs_Action *Act2Struct(IAgent::Action,
                const struct cs_State *) const; /**< find the Agent::Action struct address according to identity */
        struct cs_EnvAction *Eat2Struct(IAgent::EnvAction,
                const struct cs_State *) const; /**< find the exact strut address according to identity */
        struct cs_State *StateByEatAct(IAgent::EnvAction, IAgent::Action,
                const struct cs_State *) const; /**< find the following state according to exact and Agent::Action*/
        float MaxPayoffInEat(IAgent::EnvAction, const struct cs_State *) const; /**< maximun payoff of all following states under a specfic exact */
        float Prob(const struct cs_EnvAction*, const struct cs_State *) const; /**< probability of a exact */

        float CalStatePayoff(const struct cs_State *) const; /**< calculate payoff of a state */
        float CalActPayoff(IAgent::Action, const struct cs_State *) const; /**< calculate payoff of an Agent::Action */
};

/** implementation of environment action information */
struct cs_EnvAction
{
        IAgent::EnvAction eat; /**< eact value */
        unsigned long count; /**< eact count */
        struct cs_EnvAction *next; /**< next struct */
};

/** implementation of action information */
struct cs_Action
{
        IAgent::Action act; /**< action value */
        float payoff; /**< action payoff */
        struct cs_Action *next;
};

/** implementation of forward link information */
struct cs_ForwardArcState
{
        IAgent::EnvAction eat; /**< exact */
        IAgent::Action act; /**< action */
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
        IAgent::State st; /**< state value */
        float payoff; /**< state payoff */
        float original_payoff; /**< original payoff of state */
        unsigned long count; /**< state count */
        enum CSIAgent::SgFlag mark; /**< mark used for storage */
        struct cs_EnvAction *ealist; /**< exacts of this state */
        struct cs_Action *atlist; /**< actions of this state */
        struct cs_ForwardArcState *flist; /**< forward links */
        struct cs_BackArcState *blist; /**< backward links */
        struct cs_State *prev;
        struct cs_State *next;
};

#endif // CSIAGENT_H
