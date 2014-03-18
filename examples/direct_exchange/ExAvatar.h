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
#include <pthread.h>
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

        void exLoop();
        pthread_t threadExLoop();

        void exchange();

        void connectMAgent(OSAgent *);
        void setCps(int);
        void ava_setSps(int);

        void joinExNet(ExNetwork *);
        void leaveExNet();
        void addNeighbour(int);
        void removeNeighbour(int);
        std::set<int> getMyNeighbours() const;
        bool checkNeighbourShip(int) const;

    private:
        OSAgent *magent;
        ExNetwork *exnet;
        int cps; /**< count per sending */

        struct State_Info_Header *mergeStateInfo(
                const struct State_Info_Header *,
                const struct State_Info_Header *) const;
        void recvStateInfo();
        void sendStateInfo(int, Agent::State) const;

        static void* hook(void* args)
        { /**< hook to exLoop a class function(Launch() here) in a thread */
            reinterpret_cast<ExAvatar *>(args)->exLoop();
            return NULL;
        }
};

inline void ExAvatar::setCps(int c)
{
    cps = c;
}

inline void ExAvatar::ava_setSps(int s)
{
    setSps(s);
}

#endif /* EXMANAGER_H */
