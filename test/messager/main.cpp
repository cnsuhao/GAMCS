/*
 * main.cpp
 *
 *  Created on: Feb 17, 2014
 *      Author: andy
 */

#include "messager.h"
#include "CSMMIAgent.h"

int main(void)
{
    CSMMIAgent ma(1, 0.9, 0.01);
    Msger msger(1);
    msger.ConnectMMIAgent(&ma);
    msger.Run();
}


