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
// Created on: Feb 17, 2014
//
// -----------------------------------------------------------------------------


#ifndef MAGENT_H_
#define MAGENT_H_

#include "gimcs/Agent.h"

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
