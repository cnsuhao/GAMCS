// -----------------------------------------------------------------------------
//
// GIMCS -- Generalized Intelligence Model and Computer Simulation
//
// Copyright (C) 2013-2014, Andy Huang  <andyspider@126.com>
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// -----------------------------------------------------------------------------

#include <stdlib.h>
#include <random>
#include <math.h>
#include "gimcs/Debug.h"
#include "gimcs/GIOM.h"

namespace gimcs
{

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
GIOM::Output GIOM::Process(Input in, OSpace &alpos_outputs)
{
    OSpace restricited_outputs = Restrict(in, alpos_outputs);    // get restricted output values first
    if (restricited_outputs.Empty())    // no output generated, return an invalid GIOM::Output
        return INVALID_OUTPUT;

    gimcs_uint sz = restricited_outputs.Size();    // number of alpos_outputs
    gimcs_uint index = Random() % (sz);    // choose an output value randomly
    GIOM::Output out = restricited_outputs[index];

    // record input and output
    cur_in = in;
    cur_out = out;
    process_count++;    // inc count
    return out;
}

/** \brief Calculate the entropy of a state under restrict
 *
 * \return entropy value
 *
 */
float GIOM::SingleOutputEntropy(Input in, OSpace &alpos_outputs) const
{
    OSpace restricted_outputs = Restrict(in, alpos_outputs);
    if (restricted_outputs.Empty()) return 0.0;

    gimcs_uint sz = restricted_outputs.Size();
    return log2(sz);    // all the alpos_outputs have the same probability of occurrence
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

gimcs_uint GIOM::Random() const
{
    std::uniform_int_distribution<gimcs_uint> dist(0, GIMCS_UINT_MAX);    // act ranges: -2^63+1 ~ 2^63+1, which has a maximun number 2^64
    std::random_device rd;    // to get true random on linux, use rand("/dev/random");

    return dist(rd);
}

}    // namespace gimcs
