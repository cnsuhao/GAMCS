// -----------------------------------------------------------------------------
//
// GAMCS -- Generalized Intelligence Model and Computer Simulation
//
// Copyright (C) 2013-2014, Andy Huang  <andyspider@126.com>
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// -----------------------------------------------------------------------------
//
// Created on: Jan 21, 2014
//
// -----------------------------------------------------------------------------


#ifndef STORAGE_H_
#define STORAGE_H_
#include <string>
#include "gamcs/Agent.h"

namespace gamcs
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

        virtual int connect() = 0; /**< connect storage device */
        virtual void close() = 0; /**< close device */

        virtual struct State_Info_Header *getStateInfo(Agent::State) const = 0; /**< fetch information of a specified state value */
        virtual void addStateInfo(const struct State_Info_Header *) = 0; /**< add state information to storage */
        virtual void updateStateInfo(const struct State_Info_Header *) = 0; /**< update information of a state existing in storage */
        virtual void deleteState(Agent::State) = 0; /**< delete a state from storage */

        virtual void addMemoryInfo(const struct Memory_Info *) = 0; /**< add memory informaiton to storage */
        virtual struct Memory_Info *getMemoryInfo() const = 0; /**< fetch memory information from storage */
        virtual std::string getMemoryName() const = 0; /**< memory name */

        /* iterate all states */
        virtual Agent::State firstState() const = 0;
        virtual Agent::State nextState() const = 0;
        virtual bool hasState(Agent::State) const = 0; /**< find if a state exists in storage */

};

}    // namespace gamcs
#endif /* STORAGE_H_ */
