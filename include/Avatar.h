#ifndef AVATAR_H
#define AVATAR_H
#include <string>
#include "Agent.h"

class ParallelNet;

/**
 * Avatar Interface.
 * An avatar is an agent given meanings.
 */
class Avatar
{
    public:
        Avatar();
        Avatar(std::string);
        virtual ~Avatar();

        void Launch(); /**< launch this avatar */

        void ConnectAgent(Agent *); /**< connect to an agent */
        void SetSps(int);
        /* network related functions */
        void JoinParallelNet(ParallelNet *); /**< join a communication network */
        void LeaveParallelNet(); /**< leave a network */
        void AddNeighbour(int, int); /**< add a neighbour */
        void RemoveNeighbour(int); /**< remove a neighbour */
    protected:
        std::string name; /**< avatar's name */
        int sps; /**< number of steps per second */

        Agent *myagent; /**< connected agent */

        virtual Agent::State GetCurrentState() = 0; /**< get current state */
        virtual void PerformAction(Agent::Action) = 0; /**< perform an real action */
        virtual OutList ActionCandidates(Agent::State) = 0; /**< return a list of all action candidates of a Agent::State */
        virtual float OriginalPayoff(Agent::State); /**< original payoff of a state */

        virtual void ActualJoinParallelNet(ParallelNet *);
        virtual void ActualLeaveParallelNet();
        virtual void ActualAddNeighbour(int, int);
        virtual void ActualRemoveNeighbour(int);

    private:
        unsigned long GetCurrentTime(); /**< current time in millisecond */
        unsigned long control_step_time; /**< delta time in millisecond requested bewteen two steps */
};

inline void Avatar::SetSps(int s)
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
inline void Avatar::ConnectAgent(Agent *agt)
{
    myagent = agt;
}

#endif // AVATAR_H
