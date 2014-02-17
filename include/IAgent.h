#ifndef IAGENT_H
#define IAGENT_H
#include <stddef.h>
#include "TSGIOM.h"
#include "Debug.h"

const unsigned long INVALID_STATE = INVALID_INPUT;
const long INVALID_ACTION = INVALID_OUTPUT;

/**
 * Intelligent Agent
 */
class IAgent: public TSGIOM
{
    public:
        typedef GIOM::Input State; /**< for agent we call an input as a state */
        typedef GIOM::Output Action; /**< action as output */
        typedef long EnvAction; /**< environment action */

        IAgent();
        IAgent(int);
        IAgent(int, float, float);
        virtual ~IAgent();

        void Update(float); /**< update memory, this function will call UpdateMemory() to do the real update */

        /* set and get functions */
        void SetDiscountRate(float);
        float GetDiscountRate();
        void SetThreshold(float);
        float GetThreshold();
        void SetDegreeOfCuriosity(float);

    protected:
        int id; /**< agent Id */
        float discount_rate; /**< discount rate (0<,<1)when calculate state payoff */
        float threshold; /**< threshold used in payoff updating */

        float degree_of_curiosity; /**< degree of curiosity to try unknown actions */

        OSpace Restrict(State, OSpace &); /**< reimplement restrict using maximun payoff rule  */

        /** These two functions are implementation dependant, declared as pure virtual functions */
        virtual OSpace MaxPayoffRule(State, OSpace &) = 0; /**< implementation of maximun payoff rule */
        virtual void UpdateMemory(float) = 0; /**<  update states in memory given current state's original payoff*/
};

inline void IAgent::SetDiscountRate(float dr)
{
    // check validity
    if (dr >= 1.0 || dr < 0)    // discount rate range [0, 1)
        ERROR(
                "Agent - discount rate must be bigger than 0 and smaller than 1.0!\n");

    discount_rate = dr;
}

inline float IAgent::GetDiscountRate()
{
    return discount_rate;
}

inline void IAgent::SetThreshold(float th)
{
    if (th < 0)
    ERROR("Agent - threshold must be bigger than 0!\n");

    threshold = th;
}

inline float IAgent::GetThreshold()
{
    return threshold;
}

inline void IAgent::SetDegreeOfCuriosity(float pf)
{
    degree_of_curiosity = pf;
}

/** forward link struct for a state
 *  current state + act + eat = nst
 */
struct Forward_Link
{
        IAgent::Action act; /**< action */
        IAgent::EnvAction eat; /**< env act */
        IAgent::State nst; /**< next state */
};

/** action information */
struct Action_Info
{
        IAgent::Action act; /**< action value */
        float payoff; /**< action payoff  */
};

/** environment act information */
struct EnvAction_Info
{
        IAgent::EnvAction eat; /**< environment action value */
        unsigned long count; /**< count of experiencing times */
};

/** Header of state information, this struct can be used to share with other agents */
struct State_Info_Header
{
        IAgent::State st; /**< state value */
        float original_payoff; /**< original payoff */
        float payoff; /**< payoff */
        unsigned long count; /**< times of travelling through this state */
        int eat_num; /**< number of environment actions which have been observed */
        int act_num; /**< number of actions which have been performed */
        int lk_num; /**< number of forward links to other states */
        size_t size; /**< size of the header (in Byte) */
};

/** memory information */
struct Memory_Info
{
        float discount_rate; /**< discount rate */
        float threshold; /**< threshold */
        unsigned long state_num; /**< total number of states in memroy */
        unsigned long lk_num; /**< total number of links between states in memory */
        IAgent::State last_st; /**< last experienced state when saving memory */
        IAgent::Action last_act; /**< last performed Agent::Action when saving memory */
};

#endif // IAGENT_H
