/*
 * main.cpp
 *
 *  Created on: Feb 17, 2014
 *      Author: andy
 */

#include "messager.h"
#include "gamcs/CSOSAgent.h"

int main(void)
{
    CSOSAgent ma(1, 0.9, 0.01);
    Msger msger(1);
    msger.connectMAgent(&ma);
    msger.run();
}

