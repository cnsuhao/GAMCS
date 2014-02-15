/*
 * member.cpp
 *
 *  Created on: Feb 11, 2014
 *      Author: andy
 */

#include "member.h"

Member::Member(std::string n) :
        CSThreadIncarnation(n), count(0)
{
    position = 5;
}

Member::~Member()
{

}

IAgent::State Member::GetCurrentState()
{
    return position;
}

void Member::PerformAction(IAgent::Action act)
{
    position += act;

    if (position > 15) position = 15;
    if (position < 1) position = 1;
    return;
}

OSpace Member::ActionCandidates(IAgent::State st)
{
    if (count < 500)
    {
        OSpace acts;
        acts.clear();
        if (st == 1)    // position 1
        {
            acts.add(1);
            count++;
            return acts;
        }

        acts.add(1);
        acts.add(-1);
        count++;
        return acts;
    }
    else
        return OSpace();    // return an empty list
}

float Member::OriginalPayoff(IAgent::State st)
{
    if (st == 9)
        return 1;
    else if (st == 13)
        return 2;
    else
        return 0;
}
