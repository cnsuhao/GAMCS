#ifndef CSTHREADCOMMNET_H
#define CSTHREADCOMMNET_H
#include <set>
#include "CommNet.h"

#define MAX_MEMBER 1000     // maximun number of members in group
#define MSG_POOL_SIZE 100     // maximun number of messages a channel can store
#define DATA_SIZE 4096      // maximun size of message passed

/**
 * Channel used to transfer messages, every member has a channel.
 */
struct Channel
{
        pthread_mutex_t mutex;    // lock the channel when use it
        int ptr;    // point of current message in message pool
        int msg_num;    // number of messages in this channel
        struct Msg *msg;    // message pool
};

/**
 * Computer Simulation of CommNet used for avatars simulated as threads, in which they can communicate and share knowledge with each other.
 */
class CSThreadCommNet: public CommNet
{
    public:
        CSThreadCommNet();
        CSThreadCommNet(int);
        ~CSThreadCommNet();

        void LoadTopoFromFile(char *); /**< load topological structure of network from a file */
        void DumpTopoToFile(char *); /**< dump topological structure of network to file */

    private:
        void AddMember(int);
        void AddNeighbour(int, int, int);
        int GetNeighCommInterval(int, int);
        void ChangeNeighCommInterval(int, int, int);
        void RemoveMember(int);
        void RemoveNeighbour(int, int);

        std::set<int> GetNeighbours(int);
        std::set<int> GetAllMembers();
        bool CheckNeighbourShip(int, int);

        int Send(int, int, void *, size_t); /**< the interface members can use to send messages to a neighbour */
        int Recv(int, int, void *, size_t); /**< the interface members can use to recv message from a neighbour */

        struct Channel *GetChannel(int); /**< get channel of a specified member */
        void Notify(int); /**< notify a member of new messages recieved */

        int WrapInc(int);   /**< increase message point, when reach MSG_POOL_SIZE wrap from 0 */
        int WrapDec(int);   /**< decrease message point, when reach 0 wrap from MSG_POOL_SIZE */

        std::set<int> members; /**< save all members */
        struct Channel channels[MAX_MEMBER]; /**< channels for all members */
        struct Neigh *neighlist[MAX_MEMBER]; /**< neighbour list for all members */
};

/**
 * \brief Get one's channel.
 * \param id member id
 * \return channel address
 */
inline struct Channel *CSThreadCommNet::GetChannel(int id)
{
    return &channels[id];
}

inline std::set<int> CSThreadCommNet::GetAllMembers()
{
    return members;
}

/**
 * Message transfered between members.
 */
struct Msg
{
        int sender_id;
        char data[DATA_SIZE];    // message body
};

/**
 * Neightbour list of a member
 */
struct Neigh
{
        int id; /**< the neigh's ID */
        int interval;   /**< interval to comminucate with this neighbour */
        struct Neigh *next; /**< next neigh */
};

#endif // CSTHREADCOMMNET_H
