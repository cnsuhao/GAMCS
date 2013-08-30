#ifndef AGENT_H
#define AGENT_H

#include "PFTGIOM.h"

#define SI_MAX_SIZE 2048

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
struct State_Info_Header
{
    State st;                                   /**< state identity */
    float original_payoff;                      /**< original payoff */
    float payoff;                               /**< payoff */
    unsigned long count;
    int act_num;
    int eat_num;
    int lk_num;                        /**< count of experiencing times */
};

class Agent : public PFTGIOM
{
    public:
        Agent();
        Agent(float, float);

        virtual ~Agent();
        /* These two functions are implementation dependant, declared as pure virtual functions */
        virtual int GetStateInfo(State, void *) const = 0;         /**<  organize the information of specfic state from memory */
        virtual int MergeStateInfo(struct State_Info_Header *) = 0;       /**<  merge recieved state information to memory */
        void Update(float, State);
    protected:
        float discount_rate;                                        /**< discount rate (0<,<1)when calculate state payoff */
        float threshold;                                                /**< threshold used in payoff updating */

        vector<Action> Restrict(State, vector<Action>);     /**< reimplement restrict using maximun payoff rule  */

        virtual vector<Action> MaxPayoffRule(State, vector<Action>) = 0;        /**< implementation of maximun payoff rule */
        virtual void UpdateMemory(float, State) = 0;
    private:
};

#endif // AGENT_H
