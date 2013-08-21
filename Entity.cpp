/**********************************************************************
*	@File:
*	@Created: 2013
*	@Author: AndyHuang - andyspider@126.coom
*	@Description:
*
*	@Modify date:
***********************************************************************/
#include "Entity.h"
#include "MyAgent.h"

Entity::Entity(MyAgent ag)
{
    //ctor
    agent = ag;
    current_state = 1;
}

Entity::~Entity()
{
    //dtor
}

State Entity::GetCurrentState()
{
    return current_state;
}

void Entity::DoAction(Action act)
{
    current_state += act;
    return;
}

void Entity::Run()
{
    while(1)
    {
        State cs = GetCurrentState();
        current_state = cs;
        Action act = agent.Process(cs);
        if (act == -1)
            break;
        DoAction(act);
    }
    return;
}
