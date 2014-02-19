// -----------------------------------------------------------------------------
//
// GIMCS -- Generalized Intelligence Model and Computer Simulation
//
// Copyright (C) 2013-2014, Andy Huang  <andyspider@126.com>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//
// -----------------------------------------------------------------------------
//
// Created on: Feb 7, 2014
//
// -----------------------------------------------------------------------------


#ifndef DOTVIEWER_H_
#define DOTVIEWER_H_
#include <MemoryViewer.h>
#include <string>
#include "Agent.h"

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
        void CleanShow(); /**< show agent memory cleanly */
        void ShowState(Agent::State);
    private:
        void DotStateInfo(const struct State_Info_Header *) const;
        void CleanDotStateInfo(const struct State_Info_Header *) const; /**< show state info cleanly */
        const std::string Eat2String(Agent::EnvAction) const;

        Agent::State last_state;
        Agent::Action last_action;
};

}    // namespace gimcs
#endif /* DOTVIEWER_H_ */
