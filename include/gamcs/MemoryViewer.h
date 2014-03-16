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


#ifndef MEMORYVIEWER_H_
#define MEMORYVIEWER_H_
#include "gamcs/Debug.h"
#include "gamcs/Agent.h"

namespace gamcs
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

        void attachStorage(Storage *); /**< set storage in which memory is stored */
        virtual void show() = 0; /**< show the whole memory */
        virtual void showState(Agent::State) = 0; /**< show a specified state */

    protected:
        Storage *storage;
};

inline void MemoryViewer::attachStorage(Storage *sg)
{
    storage = sg;
}

}    // namespace gamcs
#endif /* MEMORYVIEWER_H_ */
