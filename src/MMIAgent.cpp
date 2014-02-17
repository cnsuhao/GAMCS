/*
 * MMIAgent.cpp
 *
 *  Created on: Feb 17, 2014
 *      Author: andy
 */

#include "MMIAgent.h"

MMIAgent::MMIAgent()
{
}

MMIAgent::MMIAgent(int i) :
        IAgent(i)
{
}

MMIAgent::MMIAgent(int i, float dr, float th) :
        IAgent(i, dr, th)
{
}

MMIAgent::~MMIAgent()
{
}
