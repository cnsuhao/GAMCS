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

#ifndef MEMORYVIEWER_H_
#define MEMORYVIEWER_H_
#include "gamcs/debug.h"
#include "gamcs/Agent.h"

namespace gamcs
{

class Storage;

/**
 * Interface for visualizing memory.
 */
class MemoryViewer
{
    public:
        MemoryViewer(Storage *sg = NULL) :
                storage(sg)
        {
        }

        virtual ~MemoryViewer()
        {
        }

        void attachStorage(Storage *specific_storage); /**< set the storage in which memory is stored */
        virtual void view(const char *file = NULL) = 0; /**< show the whole memory */
        virtual void viewState(Agent::State state, const char *file = NULL) = 0; /**< show a specified state */

    protected:
        Storage *storage;	/**< the storage where memory is stored */
};

/**
 * @brief Attach memory viewer to a storage, the viewer will show the memory stored in it.
 * @param sg the storage to be visualized
 */
inline void MemoryViewer::attachStorage(Storage *sg)
{
    storage = sg;
}

}    // namespace gamcs
#endif /* MEMORYVIEWER_H_ */
