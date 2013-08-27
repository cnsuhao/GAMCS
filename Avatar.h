#ifndef ENTITY_H
#define ENTITY_H
#include "MyAgent.h"
#include "SimGroup.h"

class Avatar
{
    public:
        Avatar(int);
        virtual ~Avatar();

        void Run();
        int ThreadRun();

        void SetAgent(Agent *);
        void SetGroup(Group *);
        void SetFreq(int);

        static int quit;               /**< tell all Avatar instances to quit */
    protected:
        int id;
        int freq;
        Agent *agent;
        Group *group;

        virtual State GetCurrentState() = 0;
        virtual void DoAction(Action) = 0;

        void SendStateInfo(State);
        void RecvStateInfo();

        static void* hook(void* args) {
            reinterpret_cast<Avatar *>(args)->Run();
            return NULL;
        }
    private:
};

#endif // ENTITY_H
