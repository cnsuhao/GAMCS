/*
 * MAgent.cpp
 *
 *  Created on: Feb 17, 2014
 *      Author: andy
 */

#include "MAgent.h"

MAgent::MAgent()
{
}

MAgent::MAgent(int i) :
        Agent(i)
{
}

MAgent::MAgent(int i, float dr, float th) :
        Agent(i, dr, th)
{
}

MAgent::~MAgent()
{
}
