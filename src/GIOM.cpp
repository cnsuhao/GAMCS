/**********************************************************************
*	@File:
*	@Created: 2013-8-19
*	@Author: AndyHuang - andyspider@126.coom
*	@Description:
*
*	@Modify date:
***********************************************************************/
#include <stdlib.h>
#include <time.h>
#include "Debug.h"
#include "GIOM.h"

GIOM::GIOM() : cur_in(INVALID_INPUT), cur_out(INVALID_OUTPUT)
{
    srand(time(NULL));          // random seed, here is the source of all possibilities!
}

GIOM::~GIOM()
{
}

/** \brief Restrict capacity for the GIOM.
 * Minimun restrict by default, which means NO restriction at all here.
 * \param in input identity
 * \param outlist all possible outputs for in
 * \return outputs distribution after restrict
 *
 */
std::vector<GIOM::Output> GIOM::Restrict(Input in, const std::vector<GIOM::Output> &outlist)
{
    UNUSED(in);
    return outlist; // return outlist as it is
}

/** \brief Process function of GIOM.
 * Return a random item from the restricted outputs by defaut.
 * \param in input identity
 * \return GIOM::Output
 *
 */
GIOM::Output GIOM::Process(Input in, const std::vector<GIOM::Output> &outlist)
{
    std::vector<GIOM::Output> restricited_outputs = Restrict(in, outlist); // get restricted GIOM::Output values first
    if (restricited_outputs.empty())    // no GIOM::Output generated, return an invalid GIOM::Output
        return INVALID_OUTPUT;

    int sz = restricited_outputs.size();    // number of GIOM::Output values
    int index = rand() % (sz);      // choose an GIOM::Output value randomly
    GIOM::Output out = restricited_outputs[index];

    // store input and GIOM::Output
    cur_in = in;
    cur_out = out;
    return out;
}

/** \brief Calculate the entropy of this GIOM under current restrict
 *
 * \return entropy value
 *
 */
float GIOM::Entropy()
{
    return 0.0;
}

/** \brief Update inner states.
 * Nothing to do for GIOM.
 */
void GIOM::Update()
{
    cur_in = INVALID_INPUT;
    cur_out = INVALID_OUTPUT;
    return;
}
