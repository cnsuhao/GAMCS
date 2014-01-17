#ifndef AVATAR_H
#define AVATAR_H
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

        State pre_st;   /**< previous state value */
        Action pre_act; /**< previous action value */

        virtual State GetCurrentState() = 0;    /**< get current state */
        virtual void DoAction(Action) = 0;      /**< perform an action */
        virtual vector<Action> ActionList(State) = 0;   /**< return a list of all candidate actions of a state */
        virtual State ExpectedState() = 0;      /**<  */
        virtual float OriginalPayoff(State);    /**< original payoff of a state */

        virtual void SendStateInfo(State);      /**< send information of a state to all its neighbours */
        virtual void RecvStateInfo();           /**< recieve state information from neighbours */

        static void* hook(void* args) {         /**< hook to run a class function(Run() here) in a thread */
            reinterpret_cast<Avatar *>(args)->Run();
            return NULL;
        }
    private:
};

#endif // AVATAR_H
