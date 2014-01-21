#ifndef GROUP_H
#define GROUP_H
#include <stdlib.h>
#include <vector>
/**
 * Communication Network Interface
 */
class CommNet
{
    public:
        CommNet();
        CommNet(int);
        virtual ~CommNet();

        /* build the network */
        virtual void AddMember(int) = 0; /**< add a new member to network */
        virtual void AddNeighbour(int, int) = 0; /**< add a neighbour to a member */

        virtual void RemoveMember(int) = 0; /**< remove a member from network */
        virtual void RemoveNeighbour(int, int) = 0; /**< remove a specified neighbour from a member */
        /* queries*/
        virtual std::vector<int> GetNeighbours(int) = 0; /**< get a neighbours list of a specified member */
        virtual int NumberOfMembers() = 0; /**< number of members in this network */
        virtual bool IsConnectedTo(int, int) = 0; /**< detect if a member has a specified neighbour */
        /* Communication facilities which can be used by members in this group */
        virtual int Send(int, void *, size_t) = 0; /**< send message to one's neighbour */
        virtual int Recv(int, void *, size_t) = 0; /**< recieve one's message */
    protected:
        int id; /**< network id */
};

#endif // GROUP_H
