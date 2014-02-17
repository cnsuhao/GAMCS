/*
 * MMIAgent.h
 *
 *  Created on: Feb 17, 2014
 *      Author: andy
 */

#ifndef MEIAGENT_H_
#define MEIAGENT_H_

#include <IAgent.h>

/**
 * Memory-Modifiable Intelligent Agent
 */
class MMIAgent: public IAgent
{
    public:
        MMIAgent();
        MMIAgent(int);
        MMIAgent(int, float, float);
        virtual ~MMIAgent();

        virtual struct State_Info_Header *GetStateInfo(State) const = 0;
        virtual void SetStateInfo(const struct State_Info_Header *) = 0;
        virtual void UpdateState(State) = 0;

        /* iterate all states */
        virtual State FirstState() = 0;
        virtual State NextState() = 0;
};

#endif /* MEIAGENT_H_ */
