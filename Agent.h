#ifndef AGENT_H
#define AGENT_H

#include "PFTGIOM.h"

typedef Input State;                /**< for agent we call an input state */
typedef Output Action;              /**< action for output */
typedef unsigned long ExAction;     /**< environment action, "exact" for short */

/* backward link struct for a state
*  pst + pact + peat = state concerned
*/
struct pLink
{
    State pst;          /**< previous state */
    Action pact;        /**< previous action */
    ExAction peat;      /**< previous exact */
};

/* action information */
struct Action_Info
{
    Action act;         /**< action identity */
    float payoff;       /**< action payoff  */
};

/* exact information */
struct ExAction_Info
{
    ExAction eat;           /**< exact identity */
    unsigned long count;    /**< count of experiencing times */
};

/* State information, this struct can be used to communicate with other agents */
struct State_Info
{
    State st;                                   /**< state identity */
    float original_payoff;                      /**< original payoff */
    float payoff;                               /**< payoff */
    unsigned long count;                        /**< count of experiencing times */
    int length;
    vector<struct Action_Info> actions_info;    /**< information of actions containing in this state */
    vector<struct ExAction_Info> belief;        /**< information of exacts containing in this state */
    vector<struct pLink> plinks;                /**< information of backward links of this state */
};

class Agent : public PFTGIOM
{
    public:
        Agent(int ,int);
        Agent(int ,int, float);

        virtual ~Agent();
        /* These two functions are implementation dependant, declared as pure virtual functions */
        virtual struct State_Info *GetStateInfo(State) = 0;         /**<  organize the information of specfic state from memory */
        virtual int MergeStateInfo(struct State_Info *) = 0;       /**<  merge recieved state information to memory */
    protected:
        virtual vector<Action> Restrict(State, vector<Action>);     /**< reimplement restrict using maximun payoff rule  */

        virtual vector<Action> MaxPayoffRule(State, vector<Action>) = 0;        /**< implementation of maximun payoff rule */
        virtual State ActionEffect(State, Action);
        ExAction CalExAction(State, State, Action);
        virtual float OriginalPayoff(State);                        /**< get original payoffs of each state, 1 by default */
        float discount_rate;                                        /**< discount rate (0<,<1)when calculate state payoff */
    private:
};

#endif // AGENT_H
