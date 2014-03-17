// -----------------------------------------------------------------------------
//
// GAMCS -- Generalized Intelligence Model and Computer Simulation
//
// Copyright (C) 2013-2014, Andy Huang  <andyspider@126.com>
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// -----------------------------------------------------------------------------
//
// Created on: Feb 7, 2014
//
// -----------------------------------------------------------------------------


#ifndef DOTVIEWER_H_
#define DOTVIEWER_H_
#include <string>
#include "gamcs/MemoryViewer.h"
#include "gamcs/Agent.h"

namespace gamcs
{

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

        void show();
        void showState(Agent::State);
    private:
        void dotStateInfo(const struct State_Info_Header *) const;
        const std::string act2String(Agent::Action) const;

        Agent::State last_state;
        Agent::Action last_action;
};

}    // namespace gamcs
#endif /* DOTVIEWER_H_ */