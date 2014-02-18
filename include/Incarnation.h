#ifndef INCARNATION_H
#define INCARNATION_H
#include <string>
#include "Agent.h"

class DENet;

/**
 * Incarnation of Agent.
 * An Incarnation is an agent embodied in flesh.
 */
class Incarnation
{
    public:
        Incarnation();
        Incarnation(std::string);
        virtual ~Incarnation();

        void Launch(); /**< launch this avatar */

        void ConnectAgent(Agent *); /**< connect to an agent */
        void SetSps(int);

    protected:
        std::string name; /**< avatar's name */
        int sps; /**< number of steps per second */
        unsigned long incar_loop_count;    /**< loop count */

        Agent *myagent; /**< connected agent */

        virtual Agent::State GetCurrentState() = 0; /**< get current state */
        virtual void PerformAction(Agent::Action) = 0; /**< perform an real action */
        virtual OSpace ActionCandidates(Agent::State) = 0; /**< return a list of all action candidates of a Agent::State */
        virtual float OriginalPayoff(Agent::State); /**< original payoff of a state */

    private:
        unsigned long GetCurrentTime(); /**< current time in millisecond */
        unsigned long control_step_time; /**< delta time in millisecond requested bewteen two steps */
};

inline void Incarnation::SetSps(int s)
{
    sps = s;
    if (sps <= 0)    // no control
        control_step_time = 0;
    else
        control_step_time = 1000 / sps;    // (1 / sps) * 1000
}

/**
 * \brief Connect to an agent.
 * \param agt agent to be connected
 */
inline void Incarnation::ConnectAgent(Agent *agt)
{
    myagent = agt;
}

#endif // INCARNATION_H
