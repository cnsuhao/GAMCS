#ifndef CSTHREADCOMMNET_H
#define CSTHREADCOMMNET_H
#include <vector>
#include <string>
#include "CommNet.h"

#define MAX_MEMBER 1000     // maximun number of members in group
#define CHANNEL_SIZE 100     // maximun number of messages a channel can store
#define DATA_SIZE 4096      // maximun size of message passed

/**
* Channel used to transfer messages, every member has a channel.
*/
struct Channel
{
    pthread_mutex_t mutex;  // lock the channel when use it
    int ptr;                // point of current message in message pool
    int msg_num;            // number of messages in this channel
    struct Msg *msg;        // message pool
};

/**
* Computer Simulation of CommNet used for avatars simulated as threads, in which they can communicate and share knowledge with each other.
*/
class CSThreadCommNet : public CommNet
{
    public:
        CSThreadCommNet();
        CSThreadCommNet(int);
        ~CSThreadCommNet();

        void LoadTopoFile(std::string);      /**< load topological structure of the group from a file */

        int NumberOfMembers();
        bool HasMember(int);
    private:
        void AddMember(int);
        void AddNeighbour(int, int);
        void RemoveMember(int);
        void RemoveNeighbour(int, int);
        std::vector<int> GetNeighbours(int);
        bool CheckNeighbourShip(int, int);

        int Send(int, void *, size_t);      /**< the interface members can use to send messages to others */
        int Recv(int, void *, size_t);      /**< the interface members can use to recv messages from others */

        struct Channel *GetChannel(int);    /**< get channel of a specified member */
        void Notify(int);           /**< notify a member of new messages recieved */

        std::vector<int> members;          /**< all members' Ids */
        struct Channel channels[MAX_MEMBER];        /**< channels for all members */
        struct Neigh *neighlist[MAX_MEMBER];        /**< neighbour list for all members */
};

/**
* \brief Get member number in group.
* \return number of members
*/
inline int CSThreadCommNet::NumberOfMembers()
{
    return members.size();
}

/**
* Message transfered between members.
*/
struct Msg
{
    int sender_id;          // identity of sender
    char data[DATA_SIZE];   // message body
};

/**
* Neightbour list of a member
*/
struct Neigh
{
    int id;     /**< the neigh's ID */
    struct Neigh *next;     /**< next neigh */
};

#endif // CSTHREADCOMMNET_H
