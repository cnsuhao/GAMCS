/*
 * MENet.h
 *
 *  Created on: Feb 17, 2014
 *      Author: andy
 */

#ifndef MENET_H_
#define MENET_H_
#include <stddef.h>

/**
 * Memory Exchange Network
 */
class MENet
{
    public:
        MENet();
        MENet(int);
        virtual ~MENet();

        int NumberOfMembers(); /**< number of members in this network */
        bool HasMember(int); /**< check if a member exists in network */
        /* build the network */
        virtual void AddMember(int) = 0; /**< add a new member to network */
        virtual void AddNeighbour(int, int) = 0; /**< add a neighbour to a member */

        virtual void RemoveMember(int) = 0; /**< remove a member from network */
        virtual void RemoveNeighbour(int, int) = 0; /**< remove a specified neighbour from a member */
        /* queries */
        virtual std::set<int> GetNeighbours(int) = 0; /**< get a neighbours list of a specified member */
        virtual std::set<int> GetAllMembers() = 0; /**< get all members in this network */
        virtual bool CheckNeighbourShip(int, int) = 0; /**< detect if a member has a specified neighbour */
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
inline int MENet::NumberOfMembers()
{
    return GetAllMembers().size();
}
#endif /* MENET_H_ */
