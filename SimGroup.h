#ifndef SIMGROUP_H
#define SIMGROUP_H
#include <pthread.h>
#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fstream>
#include <assert.h>
#include "Group.h"
#include "Debug.h"

using namespace std;

#define MAX_MEMBER 1000     // maximun number of members in group
#define CHANNEL_SIZE 10     // maximun number of messages a channel can store
#define DATA_SIZE 2048      // maximun size of message passed

/**
* Message transfered between members.
*/
struct Msg
{
    int sender_id;          // identity of sender
    char data[DATA_SIZE];   // message body
};

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
* Neightbour list of a member
*/
struct Neigh
{
    int id;     /**< the neigh's ID */
    struct Neigh *next;     /**< next neigh */
};

/**
* Simulation of a group of agents, in which they can communicate and share knowledge with each other.
*/
class SimGroup : public Group
{
    public:
        SimGroup();
        SimGroup(int);
        virtual ~SimGroup();

        void LoadTopo(string);      /**< load topological structure of the group from a file */
        int NumOfMembers();         /**< number of members in this group */

        int Send(int, void *, size_t);      /**< the interface members can use to send messages to others */
        int Recv(int, void *, size_t);      /**< the interface members can use to recv messages from others */
    private:
        string topofile;        /**< the file which has topological structure of the group */

        void BuildNeighsChannels();     /**< build neighbours and channels for all members */
        vector<int> GetNeighs(int);     /**< get all neighbours of a member */
        struct Channel *GetChannel(int);    /**< get channel of a specified member */
        void Notify(int);           /**< notify a member of new messages recieved */

        vector<int> members;          /**< all members' Ids */
        struct Channel channels[MAX_MEMBER];        /**< channels for all members */
        struct Neigh *neighlist[MAX_MEMBER];        /**< neighbour list for all members */
};

#endif // SIMGROUP_H
