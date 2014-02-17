/*
 * MMessager.h
 *
 *  Created on: Feb 17, 2014
 *      Author: andy
 */

#ifndef MEINCARNATION_H_
#define MEINCARNATION_H_

#include <set>
#include "Incarnation.h"
#include "IAgent.h"

class MMIAgent;
class MENet;

/**
 * Memory Messager
 */
class MMessager: private Incarnation
{
    public:
        MMessager();
        MMessager(int);
        virtual ~MMessager();

        void Run();
        void Exchange();

        void ConnectMMIAgent(MMIAgent *);
        void SetCps(int);
        void Incar_SetSps(int);

        void JoinMENet(MENet *);
        void LeaveMENet();
        void AddNeighbour(int);
        void RemoveNeighbour(int);
        std::set<int> GetMyNeighbours();
        bool CheckNeighbourShip(int);

    protected:
        int id;
        virtual IAgent::State Incar_GetCurrentState() = 0;
        virtual void Incar_PerformAction(IAgent::Action) = 0;
        virtual OSpace Incar_ActionCandidates(IAgent::State) = 0;
        virtual float Incar_OriginalPayoff(IAgent::State);

    private:
        MMIAgent *mmiagent;
        MENet *menet;
        int cps;    /**< count per sending */

        struct State_Info_Header *MergeStateInfo(
                const struct State_Info_Header *,
                const struct State_Info_Header *);
        void RecvStateInfo();
        void SendStateInfo(int, IAgent::State);

        IAgent::State GetCurrentState();
        void PerformAction(IAgent::Action);
        OSpace ActionCandidates(IAgent::State);
        float OriginalPayoff(IAgent::State);
};

inline void MMessager::SetCps(int c)
{
    cps = c;
}

inline void MMessager::Incar_SetSps(int s)
{
    SetSps(s);
}

#endif /* MEINCARNATION_H_ */
