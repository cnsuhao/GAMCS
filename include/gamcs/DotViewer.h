// -----------------------------------------------------------------------------
//
// GAMCS -- Generalized Agent Model and Computer Simulation
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
        DotViewer(Storage *storage = NULL);
        virtual ~DotViewer();

        void dump(const char *file);
        void dumpState(Agent::State state, const char *file);

    protected:
        const std::string int2String(gamcs_int value) const;

    private:
        void dotStateInfo(
                const struct State_Info_Header *state_information_header) const;

        Agent::State last_state;
        Agent::Action last_action;
};

}    // namespace gamcs
#endif /* DOTVIEWER_H_ */
