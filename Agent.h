#ifndef AGENT_H
#define AGENT_H

#include "PFTGIOM.h"

#define SI_MAX_SIZE 2048        /**< maximum size of state information */

typedef Input State;                /**< for agent we call an input as a state */
typedef Output Action;              /**< action as output */
typedef unsigned long ExAction;     /**< environment action, "exact" for short */

/** backward link struct for a state
*  pst + pact + peat = current state
*/
struct pLink
{
    State pst;          /**< previous state */
    Action pact;        /**< previous action */
    ExAction peat;      /**< previous exact */
};

/** action information */
struct Action_Info
{
    Action act;         /**< action value */
    float payoff;       /**< action payoff  */
};

/** exact information */
struct ExAction_Info
{
    ExAction eat;           /**< environment action value */
    unsigned long count;    /**< count of experiencing times */
};

/** Header of state information, this struct can be used to communicate with other agents */
struct State_Info_Header
{
    State st;                                   /**< state value */
    float original_payoff;                      /**< original payoff */
    float payoff;                               /**< payoff */
    unsigned long count;                        /**< times of travelling through this state */
    int act_num;                                /**< number of actions which have been performed */
    int eat_num;                                /**< number of environment actions which have been observed */
    int lk_num;                                 /**< number of links to other states */
};

/**
* Interface of an autonomous agent.
*/
class Agent : public PFTGIOM
{
public:
    Agent();
    Agent(float, float);

    virtual ~Agent();

    /** These two functions are implementation dependant, declared as pure virtual functions */
    virtual int GetStateInfo(State, void *) const = 0;         /**<  collect information of specified state from memory */
    virtual int MergeStateInfo(struct State_Info_Header *) = 0;       /**<  merge recieved state information into memory */
    void Update(float, State);      /**< update inner states */
protected:
    float discount_rate;                                        /**< discount rate (0<,<1)when calculate state payoff */
    float threshold;                                                /**< threshold used in payoff updating */

    vector<Action> Restrict(State, vector<Action>);     /**< reimplement restrict using maximun payoff rule  */

    virtual vector<Action> MaxPayoffRule(State, vector<Action>) = 0;        /**< implementation of maximun payoff rule */
    virtual void UpdateMemory(float, State) = 0;    /**<  update memory recursively beginning from specified state */
private:
};

#endif // AGENT_H
