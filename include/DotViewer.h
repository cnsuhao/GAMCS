/*
 * DotViewer.h
 *
 *  Created on: Feb 7, 2014
 *      Author: andy
 */

#ifndef DOTVIEWER_H_
#define DOTVIEWER_H_

#include <MemoryViewer.h>
#include <string>
#include "Agent.h"

class Storage;

/**
 * Visualizing memory in graphviz dot format
 */
class DotViewer: public MemoryViewer
{
    public:
        DotViewer();
        DotViewer(Storage *);
        virtual ~DotViewer();

        void Show();
        void CleanShow();   /**< show agent memory cleanly */
        void ShowState(Agent::State);
    private:
        void DotStateInfo(const struct State_Info_Header *);
        void CleanDotStateInfo(const struct State_Info_Header *);   /**< show state info cleanly */
        const std::string Eat2String(Agent::EnvAction);

        Agent::State last_state;
        Agent::Action last_action;
};

#endif /* DOTVIEWER_H_ */
