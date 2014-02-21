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
// Created on: Feb 17, 2014
//
// -----------------------------------------------------------------------------


#ifndef EXNET_H_
#define EXNET_H_
#include <stddef.h>

namespace gimcs
{

/**
 * Exchange Network
 */
class ExNet
{
    public:
        ExNet();
        ExNet(int);
        virtual ~ExNet();

        int NumberOfMembers() const; /**< number of members in this network */
        bool HasMember(int) const; /**< check if a member exists in network */
        /* build the network */
        virtual void AddMember(int) = 0; /**< add a new member to network */
        virtual void AddNeighbour(int, int) = 0; /**< add a neighbour to a member */

        virtual void RemoveMember(int) = 0; /**< remove a member from network */
        virtual void RemoveNeighbour(int, int) = 0; /**< remove a specified neighbour from a member */
        /* queries */
        virtual std::set<int> GetNeighbours(int) const = 0; /**< get a neighbours list of a specified member */
        virtual std::set<int> GetAllMembers() const = 0; /**< get all members in this network */
        virtual bool CheckNeighbourShip(int, int) const = 0; /**< detect if a member has a specified neighbour */
        /* Sharing facilities which can be used by members in this net */
        virtual int Send(int, int, void *, size_t) = 0; /**< some agent send message to a neighbour */
        virtual int Recv(int, int, void *, size_t) = 0; /**< some agent recieve message from a neighbour */

    protected:
        int id;
};

/**
 * \brief Get member number in group.
 * \return number of members
 */
inline int ExNet::NumberOfMembers() const
{
    return GetAllMembers().size();
}

}    // namespace gimcs
#endif /* EXNET_H_ */
