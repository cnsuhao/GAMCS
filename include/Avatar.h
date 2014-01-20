#ifndef AVATAR_H
#define AVATAR_H
#include <stdlib.h>
#include "Agent.h"
#include "CommNet.h"

/**
* Avatar Interface.
* An avatar is an agent given meanings.
*/
class Avatar
{
    public:
        Avatar();
        Avatar(int);
        virtual ~Avatar();

        void Launch();     /**< launch this avatar */

        void ConnectAgent(Agent *);     /**< connect to an agent */
        void SetFreq(int);              /**< set frequence of communication with neighbours */
        void SetCommNet(CommNet *);        /**< set which communication network this avatar is belonged to */
    protected:
        int id;         /**< avatar Id */
        int freq;       /**< communication frequence */
        Agent *agent;   /**< connected agent */
        CommNet *commnet;   /**< which network this avatar is belonged to */

        virtual Agent::Agent::State GetCurrentState() = 0;    /**< get current Agent::State */
        virtual void DoAction(Agent::Action) = 0;      /**< perform an Agent::Action */
        virtual std::vector<Agent::Action> ActionCandidates(Agent::State) = 0;   /**< return a list of all Agent::Action candidates of a Agent::State */
        virtual float OriginalPayoff(Agent::State);    /**< original payoff of a Agent::State */

        virtual void SendStateInfo(Agent::State) = 0;      /**< send information of a state to all its neighbours */
        virtual void RecvStateInfo() = 0;           /**< recieve state information from neighbours */
    private:
};

/**
 * \brief Join a commnet
 * \param grp commnet to join
 */
inline void Avatar::SetCommNet(CommNet *cn)
{
    commnet = cn;
}

/**
* \brief Set communication frequence.
*/
inline void Avatar::SetFreq(int fq)
{
    freq = fq;
}

/**
* \brief Connect to an agent.
* \param agt agent to be connected
*/
inline void Avatar::ConnectAgent(Agent *agt)
{
    agent = agt;
}

#endif // AVATAR_H
