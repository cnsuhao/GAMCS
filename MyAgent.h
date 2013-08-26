#ifndef MYAGENT_H
#define MYAGENT_H

#include <mysql/mysql.h>
#include "Agent.h"

enum m_STmark {SAVED, NEW, MODIFIED};

/* exact information */
struct m_ExAction {
    ExAction eat;               /**< exact identity */
    unsigned long count;        /**< exact count */
    struct m_ExAction *next;    /**< next struct */
};

/* action information */
struct m_Action {
    Action act;                 /**< action identity */
    float payoff;               /**< action payoff */
    struct m_Action *next;
};

/* forward link information */
struct m_ForwardArcState {
    ExAction eat;               /**< exact */
    Action act;                 /**< action */
    struct m_State *nstate;     /**< following state */
    struct m_ForwardArcState *next;
};

/* backward link information */
struct m_BackArcState {
    struct m_State *pstate;     /**< previous state */
    struct m_BackArcState *next;
};

/* state information */
struct m_State {
    State st;                   /**< state identity */
    float payoff;               /**< state payoff */
    float original_payoff;      /**< original payoff of state */
    unsigned long count;        /**< state count */
    enum m_STmark mark;         /**< mark used for saving memory to disk */
    struct m_ExAction *ealist;  /**< eaacts of this state */
    struct m_Action *atlist;    /**< actions of this state */
    struct m_ForwardArcState *flist;    /**< forward links */
    struct m_BackArcState *blist;       /**< backward links */
    struct m_State *next;
};

struct m_Memory_Info {
    unsigned long N;
    unsigned long M;
    float discount_rate;
    float threshold;
    unsigned long state_num;
    unsigned long lk_num;
};

class MyAgent : public Agent
{
    public:
        MyAgent(int, int);
        MyAgent(int, int, float, float);
        virtual ~MyAgent();

        virtual struct State_Info *GetStateInfo(State);                 /**< implementing GetStateInfo function */
        virtual int MergeStateInfo(struct State_Info *);               /**< implementing MergeStateInfo function */
        static void PrintStateInfo(struct State_Info *);
        void InitMemory();              /**< load memory from a file */
        void SetDBArgs(string, string, string, string);
    protected:
        virtual vector<Action> MaxPayoffRule(State, vector<Action>);    /**< implementing maximun payoff rule */
        float threshold;                                                /**< threshold used in payoff updating */
        unsigned long state_num;                  /**< number of states in current memory */
        unsigned long lk_num;                    /**< number of arcs */

        MYSQL *db_con;
        string db_server;
        string db_user;
        string db_password;
        string db_name;
        string db_t_stateinfo;
        string db_t_meminfo;

        int DBConnect();
        void DBClose();
        State DBNextState();
        struct State_Info *DBFetchStateInfo(State);
        int DBSearchState(State);
        void DBAddStateInfo(struct State_Info *);
        void DBUpdateStateInfo(struct State_Info *);
        void DBDeleteState(State);
        void DBAddMemoryInfo();
        struct m_Memory_Info *DBFetchMemoryInfo();

        struct m_State *head;           /**< memory point */

        struct m_State *LoadState(State);
        void SaveMemory();              /**< save memory to a file  */

        void FreeMemory();              /**< free all space of memory in computer memory*/

        void SaveState(struct m_State *, State);        /**< save current state to memory */
        void RemoveState(struct m_State *);             /**< remove "root" state */

        void LinkStates(struct m_State *, ExAction, Action, struct m_State *);  /**< link two states in memory with specfic exact and action */
        vector<Action> BestActions(struct m_State *, vector<Action>);           /**< find the best action of a state */
        struct m_State *SearchState(State);             /**< search state in memory by its identity */
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
    private:
};

#endif // MYAGENT_H
