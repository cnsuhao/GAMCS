#ifndef GROUP_H
#define GROUP_H
#include <stddef.h>     // size_t
#include <set>

/**
 * Communication Network Interface
 */
class CommNet
{
    public:
        CommNet();
        CommNet(int);
        virtual ~CommNet();

        friend class Agent;    // Avatar could access functions in CommNet

        /* network-scale queries */
        virtual int NumberOfMembers() = 0; /**< number of members in this network */
        virtual bool HasMember(int) = 0; /**< check if a member exists in network */
    protected:
        int id; /**< network id */
        /* build the network */
        virtual void AddMember(int) = 0; /**< add a new member to network */
        virtual void AddNeighbour(int, int, int) = 0; /**< add a neighbour to a member */
        virtual int GetNeighFreq(int, int) = 0;  /**< get the frequence to communicate with a neighbour of a specified member */

        virtual void RemoveMember(int) = 0; /**< remove a member from network */
        virtual void RemoveNeighbour(int, int) = 0; /**< remove a specified neighbour from a member */
        /* queries */
        virtual std::set<int> GetNeighbours(int) = 0; /**< get a neighbours list of a specified member */
        virtual bool CheckNeighbourShip(int, int) = 0; /**< detect if a member has a specified neighbour */
        /* Communication facilities which can be used by members in this group */
        virtual int Send(int, int, void *, size_t) = 0; /**< some agent send message to a neighbour */
        virtual int Recv(int, int, void *, size_t) = 0; /**< some agent recieve message from a neighbour */
};

#endif // GROUP_H
