// -----------------------------------------------------------------------------
//
// GIMCS -- Generalized Intelligence Model and Computer Simulation
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
#include "gimcs/MemoryViewer.h"
#include "gimcs/Agent.h"

namespace gimcs
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

        void Show();
        void ShowState(Agent::State);
    private:
        void DotStateInfo(const struct State_Info_Header *) const;
        const std::string Eat2String(Agent::EnvAction) const;

        Agent::State last_state;
        Agent::Action last_action;
};

}    // namespace gimcs
#endif /* DOTVIEWER_H_ */
