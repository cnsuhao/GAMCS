/*
 * CSThreadExNet.h
 *
 *  Created on: Feb 17, 2014
 *      Author: andy
 */

#ifndef CSTHREADEXNET_H_
#define CSTHREADEXNET_H_
#include <set>
#include <ExNet.h>

namespace gimcs
{

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
 *
 */
class CSThreadExNet: public ExNet
{
    public:
        enum
        {
            MSG_POOL_SIZE = 100
        };

        enum
        {
            MAX_MEMBER = 1000
        };

        enum
        {
            DATA_SIZE = 4096
        };

        CSThreadExNet();
        CSThreadExNet(int);
        virtual ~CSThreadExNet();

        void LoadTopoFromFile(char *); /**< load topological structure of network from a file */
        void DumpTopoToFile(char *); /**< dump topological structure of network to file */

    private:
        void AddMember(int);
        void AddNeighbour(int, int);
        void RemoveMember(int);
        void RemoveNeighbour(int, int);

        std::set<int> GetNeighbours(int) const;
        std::set<int> GetAllMembers() const;
        bool CheckNeighbourShip(int, int) const;

        int Send(int, int, void *, size_t); /**< the interface members can use to send messages to a neighbour */
        int Recv(int, int, void *, size_t); /**< the interface members can use to recv message from a neighbour */

        struct Channel *GetChannel(int); /**< get channel of a specified member */
        void Notify(int); /**< notify a member of new messages recieved */

        int WrapInc(int); /**< increase message point, when reach MSG_POOL_SIZE wrap from 0 */
        int WrapDec(int); /**< decrease message point, when reach 0 wrap from MSG_POOL_SIZE */

        std::set<int> members; /**< save all members */
        struct Channel channels[MAX_MEMBER]; /**< channels for all members */
        struct Neigh *neighlist[MAX_MEMBER]; /**< neighbour list for all members */
};

/**
 * \brief Get one's channel.
 * \param id member id
 * \return channel address
 */
inline struct Channel *CSThreadExNet::GetChannel(int id)
{
    return &channels[id];
}

inline std::set<int> CSThreadExNet::GetAllMembers() const
{
    return members;
}

/**
 * Message transfered between members.
 */
struct Msg
{
        int sender_id;
        char data[CSThreadExNet::DATA_SIZE];    // message body
};

/**
 * Neightbour list of a member
 */
struct Neigh
{
        int id; /**< the neigh's ID */
        struct Neigh *next; /**< next neigh */
};

}    // namespace gimcs
#endif /* CSTHREADEXNET_H_ */
