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
