#ifndef ENTITY_H
#define ENTITY_H

#include "MyAgent.h"

class Entity
{
    public:
        Entity(MyAgent);
        virtual ~Entity();

        virtual void Run();
    protected:
        MyAgent agent;
        State current_state;

        virtual State GetCurrentState();
        virtual void DoAction(Action);
    private:
};

#endif // ENTITY_H
