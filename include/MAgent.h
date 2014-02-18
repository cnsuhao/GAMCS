/*
 * MAgent.h
 *
 *  Created on: Feb 17, 2014
 *      Author: andy
 */

#ifndef MEIAGENT_H_
#define MEIAGENT_H_

#include <Agent.h>

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
        virtual void SetStateInfo(const struct State_Info_Header *) = 0;
        virtual void UpdateState(State) = 0;

        /* iterate all states */
        virtual State FirstState() = 0;
        virtual State NextState() = 0;
};

#endif /* MEIAGENT_H_ */
