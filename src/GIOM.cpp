/**********************************************************************
 *	@File: GIOM.cpp
 *	@Created: 2013-8-19
 *	@Author: AndyHuang - andyspider@126.coom
 *	@Description:
 *
 *	@Modify date:
 ***********************************************************************/
#include <stdlib.h>
#include <random>
#include "Debug.h"
#include "GIOM.h"

GIOM::GIOM() :
        cur_in(INVALID_INPUT), cur_out(INVALID_OUTPUT), process_count(0)
{
}

GIOM::~GIOM()
{
}

/** \brief Restrict capacity for the GIOM.
 * Minimun restrict by default, which means NO restriction at all here.
 * \param in input value
 * \param outputs all possible outputs for current input
 * \return output space after restrict
 *
 */
OSpace GIOM::Restrict(Input in, OSpace &outputs) const
{
    UNUSED(in);
    return outputs;    // return all outputs
}

/** \brief Process function of GIOM.
 * Return a random item from the restricted outputs by defaut.
 * \param in input identity
 * \return output
 *
 */
GIOM::Output GIOM::Process(Input in, OSpace &outputs)
{
    OSpace restricited_outputs = Restrict(in, outputs);    // get restricted output values first
    if (restricited_outputs.Empty())    // no output generated, return an invalid GIOM::Output
        return INVALID_OUTPUT;

    int sz = restricited_outputs.Size();    // number of outputs
    int index = Random() % (sz);    // choose an output value randomly
    GIOM::Output out = restricited_outputs[index];

    // record input and output
    cur_in = in;
    cur_out = out;
    process_count++;    // inc count
    return out;
}

/** \brief Calculate the entropy of this GIOM under current restrict
 *
 * \return entropy value
 *
 */
float GIOM::Entropy() const
{
    printf("This function is not implemented yet!\n");
    return 0.0;
}

/** \brief Update inner states and prepare for the next process.
 * Nothing to do for GIOM.
 */
void GIOM::Update()
{
    // clear state and prepare for the next process
    cur_in = INVALID_INPUT;
    cur_out = INVALID_OUTPUT;
    return;
}

long GIOM::Random() const
{
    std::uniform_int_distribution<long> dist(0, LONG_MAX);
    std::random_device rd;    // to get true random on linux, use rand("/dev/random");

    return dist(rd);
}
