// -----------------------------------------------------------------------------
//
// GAMCS -- Generalized Agent Model and Computer Simulation
//
// Copyright (C) 2013-2014, Andy Huang  <andyspider@126.com>
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// -----------------------------------------------------------------------------

#include "gamcs/TSGIOM.h"
#include "gamcs/debug.h"

namespace gamcs
{

TSGIOM::TSGIOM() :
		pre_in(INVALID_INPUT), pre_out(INVALID_OUTPUT)
{
}

TSGIOM::~TSGIOM()
{
}

/**
 * @brief Update the inner data of TSGIOM
 */
void TSGIOM::update()
{
	/* update time sequence */
	pre_in = cur_in;
	pre_out = cur_out;

	GIOM::update();    // as a GIOM, invoke the basic update function
	return;
}

/**
 * @brief Reimplement the constrain function.
 * No constraint at not by default just like GIOM.
 * @param input the input value
 * @param outputs the output space for the input
 * @return the sub output space after constraining
 */
OSpace TSGIOM::constrain(Input input, OSpace &outputs) const
{
	UNUSED(input);
	return outputs;
}

}    // namespace gamcs
