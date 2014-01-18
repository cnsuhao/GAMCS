#ifndef CSGROUP_H
#define CSGROUP_H

#include <string>
#include <vector>
#include "Group.h"

#define MAX_MEMBER 1000     // maximun number of members in group
#define CHANNEL_SIZE 10     // maximun number of messages a channel can store
#define DATA_SIZE 2048      // maximun size of message passed

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
* Simulation of a group of agents, in which they can communicate and share knowledge with each other.
*/
class CSGroup : public Group
{
    public:
        CSGroup();
        CSGroup(int);
        virtual ~CSGroup();

        void LoadTopo(std::string);      /**< load topological structure of the group from a file */
        int NumOfMembers();         /**< number of members in this group */

        int Send(int, const void *, size_t);      /**< the interface members can use to send messages to others */
        int Recv(int, void *, size_t);      /**< the interface members can use to recv messages from others */
    private:
        std::string topofile;        /**< the file which has topological structure of the group */

        void BuildNeighsChannels();     /**< build neighbours and channels for all members */
        std::vector<int> GetNeighs(int);     /**< get all neighbours of a member */
        struct Channel *GetChannel(int);    /**< get channel of a specified member */
        void Notify(int);           /**< notify a member of new messages recieved */

        std::vector<int> members;          /**< all members' Ids */
        struct Channel channels[MAX_MEMBER];        /**< channels for all members */
        struct Neigh *neighlist[MAX_MEMBER];        /**< neighbour list for all members */
};

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

#endif // CSGROUP_H
