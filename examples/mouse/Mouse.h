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


#ifndef MOUSE_H_
#define MOUSE_H_
#include <string>
#include "gamcs/Avatar.h"

using namespace gamcs;

class Mouse: public Avatar
{
    public:
        /** Default constructor */
        Mouse();
        /** Default destructor */
        ~Mouse();

    private:
        int position;

        Agent::State percieveState();
        void performAction(Agent::Action);
        OSpace availableActions(Agent::State);
        float originalPayoff(Agent::State);
};

#endif // MOUSE_H_
