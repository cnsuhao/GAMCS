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

Monomer::Monomer()
{
	position = 3;
}

Monomer::~Monomer()
{
	//dtor
}

Agent::State Monomer::perceiveState()
{
	printf("Monomer, %" ST_FMT "\n", position);
	return position;
}

void Monomer::performAction(Agent::Action act)
{
	if (act == 2)
		position += 1;
	else
		position -= 1;

	if (position > 10)
		position = 10;
	if (position < -10)
		position = -10;
	return;
}

/** \brief Get all outputs of each possible input.
 * By default, for a "I:N/O:M" it will return outputs with values from 1 to M for each input.
 *
 * \param in input identity
 * \return all possible outputs for the input
 *
 */
OSpace Monomer::availableActions(Agent::State st)
{
//    UNUSED(st);
	static int count;
//    UNUSED(st);
	if (count < 500)
	{
		OSpace acts;
		acts.clear();
		acts.add(2);
		acts.add(1);
		count++;
		return acts;
	}
	else
		return OSpace();    // return an empty list
}

float Monomer::originalPayoff(Agent::State st)
{
	if (st == 5)
		return 1;
	else if (st == 1)
		return 0;
	else
		return 0;
}
