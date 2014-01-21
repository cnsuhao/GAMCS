/*
 * Storage.h
 *
 *  Created on: Jan 21, 2014
 *      Author: andy
 */

#ifndef STORAGE_H_
#define STORAGE_H_
#include "Agent.h"

/**
 *  Storage Interface of memroy
 */
class Storage
{
    public:
        Storage()
        {
        }

        virtual ~Storage()
        {
        }

        virtual int Connect() = 0;      /**< connect storage device */
        virtual void Close() = 0;       /**< close device */
        virtual Agent::State StateByIndex(unsigned long) const = 0;     /**< return a state by its index in storage */
        virtual int FetchStateInfo(Agent::State, void *) const = 0;     /**< fetch information of a specified state value */
        virtual int SearchState(Agent::State) const = 0;    /**< find if a state exists in storage */
        virtual void AddStateInfo(const struct State_Info_Header *) = 0;  /**< add state information to storage */
        virtual void UpdateStateInfo(const struct State_Info_Header *) = 0;   /**< update information of a state existing in storage */
        virtual void DeleteState(Agent::State) = 0;     /**< delete a state from storage */
        virtual void AddMemoryInfo(const struct Memory_Info *) = 0;   /**< add memory informaiton to storage */
        virtual struct Memory_Info *FetchMemoryInfo() = 0;  /**< fetch memory information from storage */

};

#endif /* STORAGE_H_ */
