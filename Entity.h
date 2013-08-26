#ifndef ENTITY_H
#define ENTITY_H
#include "MyAgent.h"
#include "SimGroup.h"

class Entity
{
    public:
        Entity(int);
        virtual ~Entity();

        void Run();
        int ThreadRun();

        void SetAgent(MyAgent *);
        void SetGroup(SimGroup *);
        void SetFreq(int);
    protected:
        int id;
        int freq;
        MyAgent *agent;
        SimGroup *group;

        virtual State GetCurrentState() = 0;
        virtual void DoAction(Action) = 0;
        void RealRun();

        void SendStateInfo(State);
        void RecvStateInfo();

        static void* hook(void* args) {
            reinterpret_cast<Entity *>(args)->Run();
            return NULL;
        }
    private:
};

#endif // ENTITY_H
