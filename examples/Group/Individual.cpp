/**********************************************************************
*	@File:
*	@Created: 2013-8-21
*	@Author: AndyHuang - andyspider@126.coom
*	@Description:
*
*	@Modify date:
***********************************************************************/
#include "Individual.h"

Individual::Individual(int i):Avatar(i)
{
    position = 5;
}

Individual::~Individual()
{
    //dtor
    dbgmoreprt("~Individual\n");
}

State Individual::GetCurrentState()
{
    return position;
}

void Individual::DoAction(Action act)
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
