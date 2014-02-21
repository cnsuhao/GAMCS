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


#include "gimcs/TSGIOM.h"
#include "gimcs/Debug.h"

namespace gimcs
{

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
OSpace TSGIOM::Restrict(Input in, OSpace &outlist) const
{
    UNUSED(in);
    return outlist;
}

}    // namespace gimcs
