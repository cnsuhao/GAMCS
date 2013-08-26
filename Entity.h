#ifndef ENTITY_H
#define ENTITY_H

#include "MyAgent.h"

typedef int (*SEND_FUN) (int, void *, size_t);
typedef int (*RECV_FUN) (int, void *, size_t);

class Entity
{
    public:
        Entity(int);
        virtual ~Entity();

        void Run();
        int ThreadRun();

        void SetAgent(MyAgent *);

        void SetSendFunc(SEND_FUN);
        void SetRecvFunc(RECV_FUN);
        void SetFreq(int);
    protected:
        int id;
        int freq;
        MyAgent *agent;

        virtual State GetCurrentState() = 0;
        virtual void DoAction(Action) = 0;
        void RealRun();

        SEND_FUN Send;
        RECV_FUN Recv;
        void SendStateInfo(State);
        void RecvStateInfo();

        static void* hook(void* args) {
            reinterpret_cast<Entity *>(args)->Run();
            return NULL;
        }
    private:
};

#endif // ENTITY_H
