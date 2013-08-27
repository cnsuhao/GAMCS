/**********************************************************************
*	@File:
*	@Created: 2013-8-19
*	@Author: AndyHuang - andyspider@126.coom
*	@Description:
*
*	@Modify date:
***********************************************************************/
#include "Robot1.h"

Robot1::Robot1(int i):Entity(i)
{
    position = 2;
}

Robot1::~Robot1()
{
    //dtor
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

    if (position > 200)
        position = 200;
    if (position < 1)
        position = 1;
    return;
}
