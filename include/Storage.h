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
// Created on: Jan 21, 2014
//
// -----------------------------------------------------------------------------


#ifndef STORAGE_H_
#define STORAGE_H_
#include <string>
#include "Agent.h"

namespace gimcs
{

/**
 *  Storage Interface of memroy
 */
class Storage
{
    public:
        Storage()
        {
        }

        virtual ~Storage()
        {
        }

        virtual int Connect() = 0; /**< connect storage device */
        virtual void Close() = 0; /**< close device */

        virtual struct State_Info_Header *GetStateInfo(Agent::State) const = 0; /**< fetch information of a specified state value */
        virtual void AddStateInfo(const struct State_Info_Header *) = 0; /**< add state information to storage */
        virtual void UpdateStateInfo(const struct State_Info_Header *) = 0; /**< update information of a state existing in storage */
        virtual void DeleteState(Agent::State) = 0; /**< delete a state from storage */

        virtual void AddMemoryInfo(const struct Memory_Info *) = 0; /**< add memory informaiton to storage */
        virtual struct Memory_Info *GetMemoryInfo() const = 0; /**< fetch memory information from storage */
        virtual std::string GetMemoryName() const = 0; /**< memory name */

        /* iterate all states */
        virtual Agent::State FirstState() const = 0;
        virtual Agent::State NextState() const = 0;
        virtual bool HasState(Agent::State) const = 0; /**< find if a state exists in storage */

};

}    // namespace gimcs
#endif /* STORAGE_H_ */
