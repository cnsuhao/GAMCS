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
// Created on: Feb 17, 2014
//
// -----------------------------------------------------------------------------


#ifndef EXMANAGER_H
#define EXMANAGER_H
#include <set>
#include "Avatar.h"
#include "Agent.h"

namespace gimcs
{

class MAgent;
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

        void Run();
        void Exchange();

        void ConnectMAgent(MAgent *);
        void SetCps(int);
        void Ava_SetSps(int);

        void JoinExNet(ExNet *);
        void LeaveExNet();
        void AddNeighbour(int);
        void RemoveNeighbour(int);
        std::set<int> GetMyNeighbours() const;
        bool CheckNeighbourShip(int) const;

    protected:
        int id;
        virtual Agent::State Ava_GetCurrentState() = 0;
        virtual void Ava_PerformAction(Agent::Action) = 0;
        virtual OSpace Ava_ActionCandidates(Agent::State) = 0;
        virtual float Ava_OriginalPayoff(Agent::State);

    private:
        MAgent *magent;
        ExNet *exnet;
        int cps; /**< count per sending */
        bool quit;

        struct State_Info_Header *MergeStateInfo(
                const struct State_Info_Header *,
                const struct State_Info_Header *) const;
        void RecvStateInfo();
        void SendStateInfo(int, Agent::State) const;

        Agent::State GetCurrentState();
        void PerformAction(Agent::Action);
        OSpace ActionCandidates(Agent::State);
        float OriginalPayoff(Agent::State);
};

inline void ExManager::SetCps(int c)
{
    cps = c;
}

inline void ExManager::Ava_SetSps(int s)
{
    SetSps(s);
}

}    // namespace gimcs
#endif /* EXMANAGER_H */
