/**********************************************************************
 *	@File:
 *	@Created: 2013-8-19
 *	@Author: AndyHuang - andyspider@126.coom
 *	@Description:
 *
 *	@Modify date:
 ***********************************************************************/
#include "TSGIOM.h"
#include "Debug.h"

TSGIOM::TSGIOM() :
        pre_in(INVALID_INPUT), pre_out(INVALID_OUTPUT)
{
}

TSGIOM::~TSGIOM()
{
}

/** \brief Update inner states
 *
 *
 */
void TSGIOM::Update()
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
std::vector<GIOM::Output> TSGIOM::Restrict(Input in,
        const std::vector<GIOM::Output> &outlist)
{
    UNUSED(in);
    return outlist;
}
