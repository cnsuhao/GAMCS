/**********************************************************************
*	@File:
*	@Created: 2013-8-19
*	@Author: AndyHuang - andyspider@126.coom
*	@Description:
*
*	@Modify date:
***********************************************************************/
#include "Robot1.h"

int q = 0;

Robot1::~Robot1()
{
    //dtor
}

void Robot1::Run()
{
    while(!q)
    {
        State cs = GetCurrentState();
        printf("Current state: %ld\n", cs);
        struct State_Info *stif = ra.GetStateInfo(cs);
        if (stif != NULL)
            ra.PrintStateInfo(stif);
        Action act = ra.Process(cs);
        if (act == -1)
            break;
        DoAction(act);
    }
}

State Robot1::GetCurrentState()
{
    return position;
}

void Robot1::DoAction(Action act)
{
    if (act == 1)
        position -= 1;
    else if (act == 2)
        position += 1;

    if (position > 15)
        position = 15;
    if (position < 1)
        position = 1;
    return;
}

void quit(int signo)
{
    printf("Quit decently\n");
    q = 1;
}

int main(void)
{
    signal(SIGINT, quit);
    Robot1 rb;
    rb.Run();
}
