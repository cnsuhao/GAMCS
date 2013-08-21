/**********************************************************************
*	@File:
*	@Created: 2013-8-19
*	@Author: AndyHuang - andyspider@126.coom
*	@Description:
*
*	@Modify date:
***********************************************************************/
#include "GIOM.h"

GIOM::GIOM(int n, int m)
{
    //ctor
    N = n;
    M = m;
    srand(4);          // random seed
}

GIOM::~GIOM()
{
    //dtor
}

/** \brief Get all outputs of each possible input.
 * By default, for a "I:N/O:M" it will return outputs with values from 1 to M for each input.
 *
 * \param in input identity
 * \return all possible outputs for the input
 *
 */

vector<Output> GIOM::OutList(Input in)
{
    vector<Output> outs(M, 0);
    // 1~M
    int i = 1;
    for (vector<Output>::iterator iter = outs.begin();
    iter != outs.end(); ++iter)
    {
        *iter = i;
        i++;
    }
    return outs;
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
    return outlist;
}

/** \brief Process function of GIOM.
 * Return a random item from the restricted outputs by defaut.
 * \param in input identity
 * \return output
 *
 */

Output GIOM::Process(Input in)
{
    vector<Output> outlist;
    outlist = OutList(in);
    vector<Output> ro = Restrict(in, outlist);
    if (ro.empty())
        return -1;
    int sz = ro.size();
    int index = rand() % (sz);
    return ro[index];
}

/** \brief Calculate the entropy of this GIOM under current restrict
 *
 * \return entropy value
 *
 */

float GIOM::RectEntropy()
{
    return 0.0;
}
