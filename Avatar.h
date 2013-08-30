#ifndef ENTITY_H
#define ENTITY_H
#include "Agent.h"
#include "Group.h"

class Avatar
{
    public:
        Avatar(int);
        virtual ~Avatar();

        void Run();
        pthread_t ThreadRun();

        void ConnectAgent(Agent *);
        void JoinGroup(Group *);
        void SetFreq(int);

        static int quit;               /**< tell all Avatar instances to quit */
    protected:
        int id;
        int freq;
        Agent *agent;
        Group *group;

        State pre_st;
        Action pre_act;

        virtual State GetCurrentState() = 0;
        virtual void DoAction(Action) = 0;
        virtual vector<Action> ActionList(State) = 0;
        virtual State ExpectedState() = 0;
        virtual float OriginalPayoff(State);

        virtual void SendStateInfo(State);
        virtual void RecvStateInfo();

        static void* hook(void* args) {
            reinterpret_cast<Avatar *>(args)->Run();
            return NULL;
        }
    private:
};

#endif // ENTITY_H
