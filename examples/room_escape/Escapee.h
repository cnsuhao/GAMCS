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

#ifndef ESCAPEE_H_
#define ESCAPEE_H_
#include <string>
#include "gimcs/Avatar.h"

using namespace gimcs;

class Escapee: public Avatar
{
    public:
        Escapee(std::string name, int room = 2, int mc = 1000) :
                Avatar(name), count(0), max_count(mc), current_room(room)
        {
        }

        ~Escapee()
        {
        }

    private:
        int count;
        int max_count;
        int current_room;

        Agent::State percieveState()
        {
            printf("Current Room: %d\n", current_room);
            return current_room;
        }

        OSpace actionCandidates(Agent::State st)
        {
            OSpace acts;
            if (count < max_count)
            {
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
                count++;
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
