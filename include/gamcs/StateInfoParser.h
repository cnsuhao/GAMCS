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
// Created on: Feb 28, 2014
//
// -----------------------------------------------------------------------------

#ifndef STATEINFOPARSER_H_
#define STATEINFOPARSER_H_
#include "gamcs/Agent.h"

namespace gamcs
{

/**
 *
 */
class StateInfoParser
{
    public:
        StateInfoParser(const State_Info_Header *);
        virtual ~StateInfoParser();

        Action_Info_Header *move2Act(Agent::Action);
        Action_Info_Header *firstAct();
        Action_Info_Header *nextAct();
        EnvAction_Info *move2Eat(Agent::EnvAction);
        EnvAction_Info *firstEat();
        EnvAction_Info *nextEat();

    private:
        const State_Info_Header *my_sthd;
        unsigned char *atp;
        unsigned long act_index;
        unsigned char *eap;
        unsigned long eat_index;
};

} /* namespace gamcs */

#endif /* STATEINFOPARSER_H_ */
