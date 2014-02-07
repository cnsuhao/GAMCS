/*
 * MemoryViewer.h
 *
 *  Created on: Feb 7, 2014
 *      Author: andy
 */

#ifndef MEMORYVIEWER_H_
#define MEMORYVIEWER_H_
#include "Debug.h"

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
        virtual void Show() = 0;    /**< show memory */

    protected:
        Storage *storage;
};

#endif /* MEMORYVIEWER_H_ */
