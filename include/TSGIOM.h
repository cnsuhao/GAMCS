// -----------------------------------------------------------------------------
//
// GIMCS -- Generalized Intelligence Model and Computer Simulation
//
// Copyright (C) 2013-2014, Andy Huang  <andyspider@126.com>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//
// -----------------------------------------------------------------------------
//
// Created on: Oct 19, 2013
//
// -----------------------------------------------------------------------------


#ifndef TSGIOM_H
#define TSGIOM_H
#include "GIOM.h"

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
        virtual void Update();    // reimplement Update, add time sequence

    protected:
        virtual OSpace Restrict(Input, OSpace &) const;    // reimplement Restrict
        /* bring in the time sequence feature */
        Input pre_in; /**< previous input value */
        Output pre_out; /**< previous output value */
    private:
};

}    // namespace gimcs
#endif // TSGIOM_H
