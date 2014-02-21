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


#ifndef MEMORYVIEWER_H_
#define MEMORYVIEWER_H_
#include "gimcs/Debug.h"
#include "gimcs/Agent.h"

namespace gimcs
{

class Storage;

/**
 * Interface for visualizing stored memory.
 */
class MemoryViewer
{
    public:
        MemoryViewer();
        MemoryViewer(Storage *);
        virtual ~MemoryViewer();

        void AttachStorage(Storage *); /**< set storage in which memory is stored */
        virtual void Show() = 0; /**< show the whole memory */
        virtual void ShowState(Agent::State) = 0; /**< show a specified state */

    protected:
        Storage *storage;
};

inline void MemoryViewer::AttachStorage(Storage *sg)
{
    storage = sg;
}

}    // namespace gimcs
#endif /* MEMORYVIEWER_H_ */
