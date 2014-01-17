#ifndef MYAGENT_H
#define MYAGENT_H

#include <mysql/mysql.h>
#include <unordered_map>
#include <string>
#include "Agent.h"

/**
* Simulation of Agent, which uses computers to implement an agent.
*/
class SimAgent : public Agent
{
public:
    typedef std::unordered_map<Agent::State, void *> StatesMap; /**< hash map from state value to state struct */
    enum m_StMark {SAVED, NEW, MODIFIED};   /**< storage status of a state */

    SimAgent();
    SimAgent(float, float);
    virtual ~SimAgent();

    int GetStateInfo(Agent::State, void *) const;                 /**< implementing GetStateInfo function */
    int MergeStateInfo(const struct State_Info_Header *);               /**< implementing MergeStateInfo function */
    static void PrintStateInfo(const struct State_Info_Header *);         /**< print state information gracefully */
    void SetDBArgs(std::string, std::string, std::string, std::string);                 /**< set database related arguments */
    void InitMemory();              /**< load memory from database */
    void SaveMemory();              /**< save memory to database */

private:
    unsigned long state_num;                  /**< total number of states in memory */
    unsigned long lk_num;                    /**< total number of links between states in memory */

    std::vector<Agent::Action> MaxPayoffRule(Agent::State, const std::vector<Agent::Action> &);    /**< implementing maximun payoff rule */
    void UpdateMemory(float);            /**< implementing UpdateMemory of Agent */

    MYSQL *db_con;      /**< database connection handler */
    std::string db_server;   /**< database server address */
    std::string db_user;     /**< database username */
    std::string db_password; /**< database password */
    std::string db_name;     /**< database name */
    std::string db_t_stateinfo;  /**< table name for storing state information */
    std::string db_t_meminfo;    /**< table name for storing memory information */

    int DBConnect();    /**< connect database */
    void DBClose();     /**< close database */
    Agent::State DBStateByIndex(unsigned long) const;
    int DBFetchStateInfo(Agent::State, void *) const;
    int DBSearchState(Agent::State) const;
    void DBAddStateInfo(const struct State_Info_Header *);
    void DBUpdateStateInfo(const struct State_Info_Header *);
    void DBDeleteState(Agent::State);
    void DBAddMemoryInfo();
    struct m_Memory_Info *DBFetchMemoryInfo();
    void PrintProcess(unsigned long, unsigned long, char *) const;

    struct m_State *head;           /**< memory head*/
    StatesMap states_map;           /**< hash map from state values to state struct */
    struct m_State *cur_mst;        /**< state struct for current state value */

    void LoadState(Agent::State);               /**< fetch state struct by state value */

    void FreeMemory();              /**< free all space of memory in computer memory*/

    void RemoveState(struct m_State *);             /**< remove "root" state */

    void LinkStates(struct m_State *, Agent::EnvAction, Agent::Action, struct m_State *);  /**< link two states in memory with specfic exact and action */
    std::vector<Agent::Action> BestActions(const struct m_State *, const std::vector<Agent::Action>&);           /**< find the best action of a state */
    struct m_State *SearchState(Agent::State) const;             /**< search state in memory by its identity */
    void UpdateState(struct m_State *);             /**< update state payoff backward recursively */

    struct m_State *NewState(Agent::State);                /**< create a new state struct in memory */
    struct m_EnvAction *NewEa(Agent::EnvAction);
    struct m_Action *NewAc(Agent::Action);
    struct m_ForwardArcState *NewFas(Agent::EnvAction, Agent::Action);
    struct m_BackArcState *NewBas();
    void FreeState(struct m_State *);               /**< free a state struct */
    void FreeEa(struct m_EnvAction *);
    void FreeAc(struct m_Action *);
    void FreeFas(struct m_ForwardArcState *);
    void FreeBas(struct m_BackArcState *);

    struct m_Action *Act2Struct(Agent::Action, const struct m_State *) const;                  /**< find the Agent::Action struct address according to identity */
    struct m_EnvAction *Eat2Struct(Agent::EnvAction, const struct m_State *) const;              /**< find the exact strut address according to identity */
    struct m_State *StateByEatAct(Agent::EnvAction, Agent::Action, const struct m_State *) const;      /**< find the following state according to exact and Agent::Action*/
    float MaxPayoffInEat(Agent::EnvAction, const struct m_State *) const;                       /**< maximun payoff of all following states under a specfic exact */
    float Prob(const struct m_EnvAction*, const struct m_State *) const;                       /**< probability of a exact */

    float CalStatePayoff(const struct m_State *) const;             /**< calculate payoff of a state */
    float CalActPayoff(Agent::Action, const struct m_State *) const;       /**< calculate payoff of an Agent::Action */
};

/** implementation of environment Agent::Action information */
struct m_EnvAction
{
    Agent::EnvAction eat;               /**< eact value */
    unsigned long count;        /**< eact count */
    struct m_EnvAction *next;    /**< next struct */
};

/** implementation of Agent::Action information */
struct m_Action
{
    Agent::Action act;                 /**< Agent::Action value */
    float payoff;               /**< Agent::Action payoff */
    struct m_Action *next;
};

/** implementation of forward link information */
struct m_ForwardArcState
{
    Agent::EnvAction eat;               /**< exact */
    Agent::Action act;                 /**< Agent::Action */
    struct m_State *nstate;     /**< following state */
    struct m_ForwardArcState *next;
};

/** implementation of backward link information */
struct m_BackArcState
{
    struct m_State *pstate;     /**< previous state */
    struct m_BackArcState *next;
};

/** state information */
struct m_State
{
    Agent::State st;                   /**< state value */
    float payoff;               /**< state payoff */
    float original_payoff;      /**< original payoff of state */
    unsigned long count;        /**< state count */
    enum SimAgent::m_StMark mark;         /**< mark used for saving memory to disk */
    struct m_EnvAction *ealist;  /**< exacts of this state */
    struct m_Action *atlist;    /**< actions of this state */
    struct m_ForwardArcState *flist;    /**< forward links */
    struct m_BackArcState *blist;       /**< backward links */
    struct m_State *next;
};

/** memory information */
struct m_Memory_Info
{
    float discount_rate;    /**< discount rate */
    float threshold;        /**< threshold */
    unsigned long state_num;    /**< total number of states in memroy */
    unsigned long lk_num;       /**< total number of links between states in memory */
    Agent::State last_st;      /**< last experienced state when saving memory */
    Agent::Action last_act;    /**< last performed Agent::Action when saving memory */
};

#endif // MYAGENT_H
