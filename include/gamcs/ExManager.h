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
#include "gamcs/Agent.h"

namespace gamcs
{

class OSAgent;
class ExNet;

/**
 * Exchange Manager
 */
class ExManager: private Avatar
{
    public:
        ExManager();
        ExManager(int);
        virtual ~ExManager();

        void run();
        void exchange();

        void connectMAgent(OSAgent *);
        void setCps(int);
        void ava_setSps(int);

        void joinExNet(ExNet *);
        void leaveExNet();
        void addNeighbour(int);
        void removeNeighbour(int);
        std::set<int> getMyNeighbours() const;
        bool checkNeighbourShip(int) const;

    protected:
        int id;
        virtual Agent::State ava_getCurrentState() = 0;
        virtual void ava_performAction(Agent::Action) = 0;
        virtual OSpace ava_actionCandidates(Agent::State) = 0;
        virtual float ava_originalPayoff(Agent::State);

    private:
        OSAgent *magent;
        ExNet *exnet;
        int cps; /**< count per sending */
        bool quit;

        struct State_Info_Header *mergeStateInfo(
                const struct State_Info_Header *,
                const struct State_Info_Header *) const;
        void recvStateInfo();
        void sendStateInfo(int, Agent::State) const;

        Agent::State percieveState();
        void performAction(Agent::Action);
        OSpace availableActions(Agent::State);
        float originalPayoff(Agent::State);
};

inline void ExManager::setCps(int c)
{
    cps = c;
}

inline void ExManager::ava_setSps(int s)
{
    setSps(s);
}

}    // namespace gamcs
#endif /* EXMANAGER_H */
