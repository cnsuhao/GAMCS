#ifndef MYAGENT_H
#define MYAGENT_H

#include <mysql/mysql.h>
#include <assert.h>
#include <float.h>
#include <unordered_map>
#include "Agent.h"

typedef unordered_map<State, void *> StatesMap; /**< hash map from state value to state struct */

enum m_StMark {SAVED, NEW, MODIFIED};   /**< storage status of a state */

/** implementation of environment action information */
struct m_ExAction {
    ExAction eat;               /**< exact value */
    unsigned long count;        /**< exact count */
    struct m_ExAction *next;    /**< next struct */
};

/** implementation of action information */
struct m_Action {
    Action act;                 /**< action value */
    float payoff;               /**< action payoff */
    struct m_Action *next;
};

/** implementation of forward link information */
struct m_ForwardArcState {
    ExAction eat;               /**< exact */
    Action act;                 /**< action */
    struct m_State *nstate;     /**< following state */
    struct m_ForwardArcState *next;
};

/** implementation of backward link information */
struct m_BackArcState {
    struct m_State *pstate;     /**< previous state */
    struct m_BackArcState *next;
};

/** state information */
struct m_State {
    State st;                   /**< state value */
    float payoff;               /**< state payoff */
    float original_payoff;      /**< original payoff of state */
    unsigned long count;        /**< state count */
    enum m_StMark mark;         /**< mark used for saving memory to disk */
    struct m_ExAction *ealist;  /**< exacts of this state */
    struct m_Action *atlist;    /**< actions of this state */
    struct m_ForwardArcState *flist;    /**< forward links */
    struct m_BackArcState *blist;       /**< backward links */
    struct m_State *next;
};

/** memory information */
struct m_Memory_Info {
    float discount_rate;    /**< discount rate */
    float threshold;        /**< threshold */
    unsigned long state_num;    /**< total number of states in memroy */
    unsigned long lk_num;       /**< total number of links between states in memory */
    State last_st;      /**< last experienced state when saving memory */
    Action last_act;    /**< last performed action when saving memory */
};

/**
* Simulation of Agent, which uses computers to implement an agent.
*/
class SimAgent : public Agent
{
    public:
        SimAgent();
        SimAgent(float, float);
        virtual ~SimAgent();

        int GetStateInfo(State, void *) const;                 /**< implementing GetStateInfo function */
        int MergeStateInfo(struct State_Info_Header *);               /**< implementing MergeStateInfo function */
        static void PrintStateInfo(struct State_Info_Header *);         /**< print state information gracefully */
        void SetDBArgs(string, string, string, string);                 /**< set database related arguments */
        void InitMemory();              /**< load memory from database */
        void SaveMemory();              /**< save memory to database */

    private:
        unsigned long state_num;                  /**< total number of states in memory */
        unsigned long lk_num;                    /**< total number of links between states in memory */

        vector<Action> MaxPayoffRule(State, vector<Action>);    /**< implementing maximun payoff rule */
        void UpdateMemory(float);            /**< implementing UpdateMemory of Agent */

        MYSQL *db_con;      /**< database connection handler */
        string db_server;   /**< database server address */
        string db_user;     /**< database username */
        string db_password; /**< database password */
        string db_name;     /**< database name */
        string db_t_stateinfo;  /**< table name for storing state information */
        string db_t_meminfo;    /**< table name for storing memory information */

        int DBConnect();    /**< connect database */
        void DBClose();     /**< close database */
        State DBStateByIndex(unsigned long);
        int DBFetchStateInfo(State, void *);
        int DBSearchState(State);
        void DBAddStateInfo(struct State_Info_Header *);
        void DBUpdateStateInfo(struct State_Info_Header *);
        void DBDeleteState(State);
        void DBAddMemoryInfo();
        struct m_Memory_Info *DBFetchMemoryInfo();
        void PrintProcess(unsigned long, unsigned long, char *);

        struct m_State *head;           /**< memory head*/
        StatesMap states_map;           /**< hash map from state values to state struct */
        struct m_State *cur_mst;        /**< state struct for current state value */

        void LoadState(State);               /**< fetch state struct by state value */

        void FreeMemory();              /**< free all space of memory in computer memory*/

        void RemoveState(struct m_State *);             /**< remove "root" state */

        void LinkStates(struct m_State *, ExAction, Action, struct m_State *);  /**< link two states in memory with specfic exact and action */
        vector<Action> BestActions(struct m_State *, vector<Action>);           /**< find the best action of a state */
        struct m_State *SearchState(State) const;             /**< search state in memory by its identity */
        void UpdateState(struct m_State *);             /**< update state payoff backward recursively */

        struct m_State *NewState(State);                /**< create a new state struct in memory */
        struct m_ExAction *NewEa(ExAction);
        struct m_Action *NewAc(Action);
        struct m_ForwardArcState *NewFas(ExAction, Action);
        struct m_BackArcState *NewBas();
        void FreeState(struct m_State *);               /**< free a state struct */
        void FreeEa(struct m_ExAction *);
        void FreeAc(struct m_Action *);
        void FreeFas(struct m_ForwardArcState *);
        void FreeBas(struct m_BackArcState *);

        struct m_Action *Act2Struct(Action, struct m_State *);                  /**< find the action struct address according to identity */
        struct m_ExAction *Eat2Struct(ExAction, struct m_State *);              /**< find the exact strut address according to identity */
        struct m_State *StateByEatAct(ExAction, Action, struct m_State *);      /**< find the following state according to exact and action*/
        float MaxPayoffInEat(ExAction, struct m_State *);                       /**< maximun payoff of all following states under a specfic exact */
        float Prob(struct m_ExAction*, struct m_State *);                       /**< probability of a exact */

        ExAction CalExAction(State, State, Action);         /**< construct a exact identity from previous state, current state and previous action */
        float CalStatePayoff(struct m_State *);             /**< calculate payoff of a state */
        float CalActPayoff(Action, struct m_State *);       /**< calculate payoff of an action */
};

#endif // MYAGENT_H
