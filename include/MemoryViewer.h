/*
 * MemoryViewer.h
 *
 *  Created on: Feb 7, 2014
 *      Author: andy
 */

#ifndef MEMORYVIEWER_H_
#define MEMORYVIEWER_H_
#include "Debug.h"
#include "Agent.h"

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

        void SetStorage(Storage *);     /**< set storage in which memory is stored */
        virtual void Show() = 0;    /**< show the whole memory */
        virtual void ShowState(Agent::State) = 0;   /**< show a specified state */

    protected:
        Storage *storage;
};

inline void MemoryViewer::SetStorage(Storage *sg)
{
    storage = sg;
}
#endif /* MEMORYVIEWER_H_ */
