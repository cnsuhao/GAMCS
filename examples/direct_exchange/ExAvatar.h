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
// Created on: Feb 17, 2014
//
// -----------------------------------------------------------------------------

#ifndef EXMANAGER_H
#define EXMANAGER_H
#include <set>
#include "gamcs/Avatar.h"
#include "gamcs/OSAgent.h"

using namespace gamcs;

class ExNetwork;

/**
 * Exchange Manager
 */
class ExAvatar: public Avatar
{
    public:
        ExAvatar();
        ExAvatar(int);
        virtual ~ExAvatar();

        int exStep();

        void exchange();

        void connectOSAgent(OSAgent *);
        void setCps(int);

        void joinExNet(ExNetwork *);
        void leaveExNet();
        void addNeighbour(int);
        void removeNeighbour(int);
        std::set<int> getMyNeighbours() const;
        bool checkNeighbourShip(int) const;

    private:
        OSAgent *osagent;
        ExNetwork *exnet;
        int cps; /**< count per sending */

        struct State_Info_Header *mergeStateInfo(
                const struct State_Info_Header *,
                const struct State_Info_Header *) const;
        void recvStateInfo();
        void sendStateInfo(int, Agent::State) const;
};

inline void ExAvatar::setCps(int c)
{
    cps = c;
}

#endif /* EXMANAGER_H */
