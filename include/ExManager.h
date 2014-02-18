/*
 * ExManager.h
 *
 *  Created on: Feb 17, 2014
 *      Author: andy
 */

#ifndef MEINCARNATION_H_
#define MEINCARNATION_H_

#include <set>
#include "Incarnation.h"
#include "Agent.h"

class MAgent;
class ExNet;

/**
 * Exchange Manager
 */
class ExManager: private Incarnation
{
    public:
        ExManager();
        ExManager(int);
        virtual ~ExManager();

        void Run();
        void Exchange();

        void ConnectMAgent(MAgent *);
        void SetCps(int);
        void Incar_SetSps(int);

        void JoinExNet(ExNet *);
        void LeaveExNet();
        void AddNeighbour(int);
        void RemoveNeighbour(int);
        std::set<int> GetMyNeighbours();
        bool CheckNeighbourShip(int);

    protected:
        int id;
        virtual Agent::State Incar_GetCurrentState() = 0;
        virtual void Incar_PerformAction(Agent::Action) = 0;
        virtual OSpace Incar_ActionCandidates(Agent::State) = 0;
        virtual float Incar_OriginalPayoff(Agent::State);

    private:
        MAgent *magent;
        ExNet *exnet;
        int cps;    /**< count per sending */
        bool quit;

        struct State_Info_Header *MergeStateInfo(
                const struct State_Info_Header *,
                const struct State_Info_Header *);
        void RecvStateInfo();
        void SendStateInfo(int, Agent::State);

        Agent::State GetCurrentState();
        void PerformAction(Agent::Action);
        OSpace ActionCandidates(Agent::State);
        float OriginalPayoff(Agent::State);
};

inline void ExManager::SetCps(int c)
{
    cps = c;
}

inline void ExManager::Incar_SetSps(int s)
{
    SetSps(s);
}

#endif /* MEINCARNATION_H_ */
