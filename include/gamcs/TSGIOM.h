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
//
// Created on: Oct 19, 2013
//
// -----------------------------------------------------------------------------

#ifndef TSGIOM_H
#define TSGIOM_H
#include "gamcs/GIOM.h"

namespace gamcs
{

/**
 * Time-Sequential Generalized Input/Output Model.
 * TSGIOM is a kind of GIOM which has the capability to record time sequence.
 */
class TSGIOM: public GIOM
{
	public:
		TSGIOM();
		virtual ~TSGIOM();
		virtual void update();

	protected:
		virtual OSpace constrain(Input input, OSpace &available_outputs) const;
		/* bring in the time sequence feature */
		Input pre_in; /**< the recorded previous input */
		Output pre_out; /**< the recorded previous output */
	private:
};

}    // namespace gamcs
#endif // TSGIOM_H
