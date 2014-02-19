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
// Created on: Feb 17, 2014
//
// -----------------------------------------------------------------------------


#ifndef MAGENT_H_
#define MAGENT_H_

#include <Agent.h>

namespace gimcs
{

/**
 *  Manipulatable Agent
 */
class MAgent: public Agent
{
    public:
        MAgent();
        MAgent(int);
        MAgent(int, float, float);
        virtual ~MAgent();

        virtual struct State_Info_Header *GetStateInfo(State) const = 0;
        virtual void AddStateInfo(const struct State_Info_Header *) = 0;
        virtual void UpdateStateInfo(const struct State_Info_Header *) = 0;
        virtual void DeleteState(State) = 0;

        virtual void UpdateState(State) = 0;

        /* iterate all states */
        virtual State FirstState() const = 0;
        virtual State NextState() const = 0;
        virtual bool HasState(State) const = 0;
};

}    // namespace gimcs

#endif /* MAGENT_H_ */
