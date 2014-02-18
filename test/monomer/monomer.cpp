/**********************************************************************
 *	@File:
 *	@Created: 2013-8-19
 *	@Author: AndyHuang - andyspider@126.coom
 *	@Description:
 *
 *	@Modify date:
 ***********************************************************************/
#include <string>
#include "monomer.h"

Monomer::Monomer(std::string n) :
        Incarnation(n)
{
    position = 12;
}

Monomer::~Monomer()
{
    //dtor
}

Agent::State Monomer::GetCurrentState()
{
    return position;
}

void Monomer::PerformAction(Agent::Action act)
{
    position += act;

    if (position > 15) position = 15;
    if (position < 1) position = 1;
    return;
}

/** \brief Get all outputs of each possible input.
 * By default, for a "I:N/O:M" it will return outputs with values from 1 to M for each input.
 *
 * \param in input identity
 * \return all possible outputs for the input
 *
 */
OSpace Monomer::ActionCandidates(Agent::State st)
{
//    UNUSED(st);
    static int count;
//    UNUSED(st);
    if (count < 300)
    {
        OSpace acts;
        acts.Clear();
        if (st == 1)
            acts.Add(1);
        else if (st == 15)
            acts.Add(-1);
        else
        {
            acts.Add(1);
            acts.Add(-1);
        }
        count++;
        return acts;
    }
    else
        return OSpace();    // return an empty list
}

float Monomer::OriginalPayoff(Agent::State st)
{
    return -( st * 0.1);
//    if (st == 9)
//        return 1;
//    else if (st == 13)
//        return 2;
//    else
//        return 0;
}
