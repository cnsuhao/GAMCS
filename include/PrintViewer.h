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


#ifndef PRINTVEWER_H_
#define PRINTVEWER_H_
#include <MemoryViewer.h>
#include "Agent.h"

namespace gimcs
{

class Storage;

/**
 * Visualizing memory in print format.
 */
class PrintViewer: public MemoryViewer
{
    public:
        PrintViewer();
        PrintViewer(Storage *);
        virtual ~PrintViewer();

        void Show();
        void ShowState(Agent::State);
    private:
        void PrintStateInfo(const struct State_Info_Header *) const;
};

}    // namespace gimcs
#endif /* PRINTVEWER_H_ */
