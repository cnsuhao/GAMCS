/**********************************************************************
*	@File:
*	@Created: 2013-8-19
*	@Author: AndyHuang - andyspider@126.coom
*	@Description:
*
*	@Modify date:
***********************************************************************/
#include "GIOM.h"

GIOM::GIOM()
{
    //ctor
    cur_in = -1;
    cur_out = -1;
    srand(time(NULL));          // random seed
}

GIOM::~GIOM()
{
    //dtor
}

/** \brief Restrict capacity for the GIOM.
 * Minimun restrict by default.
 * \param in input identity
 * \param outlist all possible outputs for in
 * \return outputs distribution after restrict
 *
 */
vector<Output> GIOM::Restrict(Input in, vector<Output> outlist)
{
    UNUSED(in);
    dbgmoreprt("enter GIOM restrict\n\n");
    return outlist;
}

/** \brief Process function of GIOM.
 * Return a random item from the restricted outputs by defaut.
 * \param in input identity
 * \return output
 *
 */

Output GIOM::Process(Input in, vector<Output> outlist)
{
    vector<Output> restricited_outputs = Restrict(in, outlist);
    if (restricited_outputs.empty())
        return -1;
    int sz = restricited_outputs.size();
    int index = rand() % (sz);
    Output out = restricited_outputs[index];

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

void GIOM::Update()
{
    cur_in = -1;
    cur_out = -1;
    return;
}
