/**********************************************************************
*	@File:
*	@Created: 2013-8-19
*	@Author: AndyHuang - andyspider@126.coom
*	@Description:
*
*	@Modify date:
***********************************************************************/
#include "PFTGIOM.h"
#include "Debug.h"

PFTGIOM::PFTGIOM(): pre_in(INVALID_INPUT), pre_out(INVALID_OUTPUT)
{
}

PFTGIOM::~PFTGIOM()
{
}

/** \brief Reimplement Process function.
 * Append time sequence functionaliy to GIOM.
 * \param in input value
 * \return GIOM::Output value
 *
 */
GIOM::Output PFTGIOM::Process(Input in, const std::vector<GIOM::Output> &outlist)
{
    GIOM::Output out = GIOM::Process(in, outlist);
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
std::vector<GIOM::Output> PFTGIOM::Restrict(Input in, const std::vector<GIOM::Output> &outlist)
{
    UNUSED(in);
    return outlist;
}
