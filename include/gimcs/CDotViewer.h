/*
 * CDotViewer.h
 *
 *  Created on: Feb 21, 2014
 *      Author: andy
 */

#ifndef CDOTVIEWER_H_
#define CDOTVIEWER_H_

#include "gimcs/DotViewer.h"
#include "gimcs/MemoryViewer.h"
#include "gimcs/Agent.h"

namespace gimcs
{

/**
 * Clean Dot Viewer
 */
class CDotViewer: public DotViewer
{
    public:
        CDotViewer();
        CDotViewer(Storage *);
        virtual ~CDotViewer();

        void Show(); /**< show agent memory cleanly */

    private:
        void CleanDotStateInfo(const struct State_Info_Header *) const; /**< show state info cleanly */

        Agent::State last_state;
        Agent::Action last_action;
};

} /* namespace gimcs */

#endif /* CDOTVIEWER_H_ */
