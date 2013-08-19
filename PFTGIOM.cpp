/**********************************************************************
*	@File:
*	@Created: 2013-8-19
*	@Author: AndyHuang - andyspider@126.coom
*	@Description:
*
*	@Modify date:
***********************************************************************/
#include "PFTGIOM.h"

PFTGIOM::PFTGIOM(int n, int m):GIOM(n, m)
{
    //ctor
    pre_in = -1;
    pre_out = -1;
}

PFTGIOM::~PFTGIOM()
{
    //dtor
}

/** \brief Reimplement Process function.
 * Append time sequence functionaliy to GIOM.
 * \param in input identity
 * \return output
 *
 */

Output PFTGIOM::Process(Input in)
{
    Output out = GIOM::Process(in);
    /* update in time sequence */
    pre_in = in;
    pre_out = out;
    return out;
}

/** \brief Reimplement Restrict function.
 * Return all possible outputs by default.
 * \param in input identity
 * \param outlist all possible outputs for in
 * \return outputs distribution after restricting
 *
 */

vector<Output> PFTGIOM::Restrict(Input in, vector<Output> outlist)
{
    return outlist;
}
