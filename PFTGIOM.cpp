/**********************************************************************
*	@File:
*	@Created: 2013-8-19
*	@Author: AndyHuang - andyspider@126.coom
*	@Description:
*
*	@Modify date:
***********************************************************************/
#include "PFTGIOM.h"

PFTGIOM::PFTGIOM(): pre_in(INVALID_VALUE), pre_out(INVALID_VALUE)
{
}

PFTGIOM::~PFTGIOM()
{
}

/** \brief Reimplement Process function.
 * Append time sequence functionaliy to GIOM.
 * \param in input value
 * \return output value
 *
 */
Output PFTGIOM::Process(Input in, vector<Output> outlist)
{
    Output out = GIOM::Process(in, outlist);
    return out;
}

/** \brief Update inner states
 *
 *
 */
void PFTGIOM::Update()
{
    /* update time sequence */
    pre_in = cur_in;
    pre_out = cur_out;

    GIOM::Update();
    return;
}

/** \brief Reimplement Restrict function.
 * Return all possible outputs by default.
 * \param in input value
 * \param outlist all possible outputs for in
 * \return outputs distribution after restricting
 *
 */
vector<Output> PFTGIOM::Restrict(Input in, vector<Output> outlist)
{
    UNUSED(in);
    return outlist;
}
