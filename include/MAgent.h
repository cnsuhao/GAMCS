/*
 * MAgent.h
 *
 *  Created on: Feb 17, 2014
 *      Author: andy
 */

#ifndef MAGENT_H_
#define MAGENT_H_

#include <Agent.h>

namespace gimcs
{

/**
 *  Manipulatable Agent
 */
class MAgent: public Agent
{
    public:
        MAgent();
        MAgent(int);
        MAgent(int, float, float);
        virtual ~MAgent();

        virtual struct State_Info_Header *GetStateInfo(State) const = 0;
        virtual void AddStateInfo(const struct State_Info_Header *) = 0;
        virtual void UpdateStateInfo(const struct State_Info_Header *) = 0;
        virtual void DeleteState(State) = 0;

        virtual void UpdateState(State) = 0;

        /* iterate all states */
        virtual State FirstState() const = 0;
        virtual State NextState() const = 0;
        virtual bool HasState(State) const = 0;
};

}    // namespace gimcs

#endif /* MAGENT_H_ */
