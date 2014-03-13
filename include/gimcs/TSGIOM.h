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
//
// Created on: Oct 19, 2013
//
// -----------------------------------------------------------------------------


#ifndef TSGIOM_H
#define TSGIOM_H
#include "gimcs/GIOM.h"

namespace gimcs
{

/**
 * Time-Sequential Generalized Input Output Model
 */
class TSGIOM: public GIOM
{
    public:
        TSGIOM();
        virtual ~TSGIOM();
        virtual void update();    // reimplement update, add time sequence

    protected:
        virtual OSpace constrain(Input, OSpace &) const;    // reimplement constrain
        /* bring in the time sequence feature */
        Input pre_in; /**< previous input value */
        Output pre_out; /**< previous output value */
    private:
};

}    // namespace gimcs
#endif // TSGIOM_H
