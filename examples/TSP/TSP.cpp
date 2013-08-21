/**********************************************************************
*	@File:
*	@Created: 2013
*	@Author: AndyHuang - andyspider@126.coom
*	@Description:
*
*	@Modify date:
***********************************************************************/
#include "TSP.h"

void print_vector(vector<int> ivec)
{
    for(vector<int>::iterator it = ivec.begin();
    it != ivec.end(); ++it)
        printf("%d, ", *it);

    printf("\n");
    return;
}

TSP::~TSP()
{
    //dtor
}

State TSP::GetCurrentState()
{
    return route;
}

void TSP::DoAction(Action act)
{
    route &= 0xff;
    route <<= 8;
    route |= act;
    return;
}

void TSP::Run()
{
repeat:
    while(1)
    {
        State cs = GetCurrentState();
        Action act = ta.Process(cs);
        if (act == -1)
            break;
        DoAction(act);

        struct State_Info *stif = ta.GetStateInfo(cs);
        if (stif != NULL)
            printf("Gain: %0.2f\n", stif->payoff);
        printf("Action: %ld\n\n", act);

    }

    printf("=========================================\n");
    getchar();
    route = 0x01;
    ta.Init();
    goto repeat;

    printf("exit\n");
}

int main(void)
{
    TSP tsp;
    tsp.Run();
}
