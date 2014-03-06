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

        int numberOfMembers() const; /**< number of members in this network */
        bool hasMember(int) const; /**< check if a member exists in network */
        /* build the network */
        virtual void addMember(int) = 0; /**< add a new member to network */
        virtual void addNeighbour(int, int) = 0; /**< add a neighbour to a member */

        virtual void removeMember(int) = 0; /**< remove a member from network */
        virtual void removeNeighbour(int, int) = 0; /**< remove a specified neighbour from a member */
        /* queries */
        virtual std::set<int> getNeighbours(int) const = 0; /**< get a neighbours list of a specified member */
        virtual std::set<int> getAllMembers() const = 0; /**< get all members in this network */
        virtual bool checkNeighbourShip(int, int) const = 0; /**< detect if a member has a specified neighbour */
        /* Sharing facilities which can be used by members in this net */
        virtual int send(int, int, void *, size_t) = 0; /**< some agent send message to a neighbour */
        virtual int recv(int, int, void *, size_t) = 0; /**< some agent recieve message from a neighbour */

    protected:
        int id;
};

/**
 * \brief Get member number in group.
 * \return number of members
 */
inline int ExNet::numberOfMembers() const
{
    return getAllMembers().size();
}

}    // namespace gimcs
#endif /* EXNET_H_ */
