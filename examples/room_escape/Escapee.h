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

#ifndef ESCAPEE_H_
#define ESCAPEE_H_
#include "gamcs/Avatar.h"

using namespace gamcs;

class Escapee: public Avatar
{
    public:
        Escapee(int room = 2) :
                current_room(room)
        {
        }

        ~Escapee()
        {
        }

    private:
        int current_room;

        Agent::State perceiveState()
        {
            printf("Current Room: %d\n", current_room);
            return current_room;
        }

        OSpace availableActions(Agent::State st)
        {
            OSpace acts;
            switch (st)
            {
                case 0:
                    acts.add(4);
                    break;
                case 1:
                    acts.add(3);
                    acts.add(5);
                    break;
                case 2:
                    acts.add(3);
                    break;
                case 3:
                    acts.add(1);
                    acts.add(4);
                    acts.add(2);
                    break;
                case 4:
                    acts.add(5);
                    acts.add(0);
                    acts.add(3);
                    break;
                case 5:
                    acts.add(1);
                    acts.add(4);
                    acts.add(5);
                    break;
            }

            return acts;
        }

        void performAction(Agent::Action act)
        {
            current_room = (Agent::State) act;
        }

        float originalPayoff(Agent::State st)
        {
            if (st == 5)
                return 100;
            else
                return 0;
        }
};

#endif
