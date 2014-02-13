#ifndef PARALLELNET_H
#define PARALLELNET_H
#include <stddef.h>     // size_t
#include <set>

/**
 * Parallel Network Interface
 */
class ParallelNet
{
    public:
        ParallelNet();
        ParallelNet(int);
        virtual ~ParallelNet();

        friend class Agent;    // Avatar need to access functions in ParallelNet

        /* pub can only do network-scale queries */
        int NumberOfMembers(); /**< number of members in this network */
        bool HasMember(int); /**< check if a member exists in network */

    protected:
        int id; /**< network id */

        /* build the network */
        virtual void AddMember(int) = 0; /**< add a new member to network */
        virtual void AddNeighbour(int, int, int) = 0; /**< add a neighbour to a member */
        virtual int GetNeighSharingInterval(int, int) = 0; /**< get the interval to share with a neighbour of a specified member */
        virtual void ChangeNeighSharingInterval(int, int, int) = 0; /**< change the interval to share with a neighbour */

        virtual void RemoveMember(int) = 0; /**< remove a member from network */
        virtual void RemoveNeighbour(int, int) = 0; /**< remove a specified neighbour from a member */
        /* queries */
        virtual std::set<int> GetNeighbours(int) = 0; /**< get a neighbours list of a specified member */
        virtual std::set<int> GetAllMembers() = 0; /**< get all members in this network */
        virtual bool CheckNeighbourShip(int, int) = 0; /**< detect if a member has a specified neighbour */
        /* Sharing facilities which can be used by members in this net */
        virtual int Send(int, int, void *, size_t) = 0; /**< some agent send message to a neighbour */
        virtual int Recv(int, int, void *, size_t) = 0; /**< some agent recieve message from a neighbour */
};

/**
 * \brief Get member number in group.
 * \return number of members
 */
inline int ParallelNet::NumberOfMembers()
{
    return GetAllMembers().size();
}

#endif // PARALLELNET_H
