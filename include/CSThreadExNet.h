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
        void DumpTopoToFile(char *) const; /**< dump topological structure of network to file */

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
