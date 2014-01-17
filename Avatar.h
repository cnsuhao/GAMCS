#ifndef AVATAR_H
#define AVATAR_H
#include <stdlib.h>
#include "Agent.h"
#include "Group.h"

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

        void Run();     /**< run this avatar */
        pthread_t ThreadRun();      /**< if it's a group, run its every member in a thread */

        void ConnectAgent(Agent *);     /**< connect to an agent */
        void JoinGroup(Group *);        /**< join a group */
        void SetFreq(int);              /**< set frequence of communication with neighbours */

        static bool quit;               /**< tell all Avatar instances to quit, it's class scope */
    protected:
        int id;         /**< avatar Id */
        int freq;       /**< communication frequence */
        Agent *agent;   /**< connected agent */
        Group *group;   /**< my group */

        virtual Agent::Agent::State GetCurrentState() = 0;    /**< get current Agent::State */
        virtual void DoAction(Agent::Action) = 0;      /**< perform an Agent::Action */
        virtual std::vector<Agent::Action> ActionCandidates(Agent::State) = 0;   /**< return a list of all Agent::Action candidates of a Agent::State */
        virtual float OriginalPayoff(Agent::State);    /**< original payoff of a Agent::State */

        virtual void SendStateInfo(Agent::State);      /**< send information of a Agent::State to all its neighbours */
        virtual void RecvStateInfo();           /**< recieve Agent::State information from neighbours */

        static void* hook(void* args) {         /**< hook to run a class function(Run() here) in a thread */
            reinterpret_cast<Avatar *>(args)->Run();
            return NULL;
        }
    private:
};

#endif // AVATAR_H
