/**********************************************************************
*	@File:
*	@Created: 2013-8-19
*	@Author: AndyHuang - andyspider@126.coom
*	@Description:
*
*	@Modify date:
***********************************************************************/
#include "GIOM.h"

GIOM::GIOM() : cur_in(INVALID_VALUE), cur_out(INVALID_VALUE)
{
    srand(time(NULL));          // random seed, true random values needed
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
vector<Output> GIOM::Restrict(Input in, vector<Output> outlist)
{
    UNUSED(in);
    return outlist; // return outlist as it is
}

/** \brief Process function of GIOM.
 * Return a random item from the restricted outputs by defaut.
 * \param in input identity
 * \return output
 *
 */
Output GIOM::Process(Input in, vector<Output> outlist)
{
    vector<Output> restricited_outputs = Restrict(in, outlist); // get restricted output values first
    if (restricited_outputs.empty())    // no output generated, return an invalid output
        return INVALID_VALUE;

    int sz = restricited_outputs.size();    // number of output values
    int index = rand() % (sz);      // choose an output value randomly
    Output out = restricited_outputs[index];

    // store states
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
    cur_in = INVALID_VALUE;
    cur_out = INVALID_VALUE;
    return;
}
