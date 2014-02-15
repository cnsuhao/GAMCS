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
#include "IAgent.h"

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
        void ShowState(IAgent::State);
    private:
        void DotStateInfo(const struct State_Info_Header *);
        void CleanDotStateInfo(const struct State_Info_Header *);   /**< show state info cleanly */
        const std::string Eat2String(IAgent::EnvAction);

        IAgent::State last_state;
        IAgent::Action last_action;
};

#endif /* DOTVIEWER_H_ */
