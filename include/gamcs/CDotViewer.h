/*
 * CDotViewer.h
 *
 *  Created on: Feb 21, 2014
 *      Author: andy
 */

#ifndef CDOTVIEWER_H_
#define CDOTVIEWER_H_

#include "gamcs/DotViewer.h"
#include "gamcs/MemoryViewer.h"
#include "gamcs/Agent.h"

namespace gamcs
{

/**
 * Clean Dot Viewer
 */
class CDotViewer: public DotViewer
{
    public:
        CDotViewer(Storage *storage = NULL);
        virtual ~CDotViewer();

        void show(); /**< show agent memory cleanly */

    private:
        void cleanDotStateInfo(
                const struct State_Info_Header * state_information_header) const; /**< show state info cleanly */

        Agent::State last_state;
        Agent::Action last_action;
};

} /* namespace gamcs */

#endif /* CDOTVIEWER_H_ */
