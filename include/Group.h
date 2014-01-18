#ifndef GROUP_H
#define GROUP_H
#include <stdlib.h>
/**
* Group of agents
*/
class Group
{
    public:
        Group();
        Group(int);
        virtual ~Group();

        /* Communication facilities which can be used by members in this group */
        virtual int Send(int, const void *, size_t) = 0;  /**< send message to one's neighbour */
        virtual int Recv(int, void *, size_t) = 0;  /**< recieve one's message */
    protected:
        int id;     /**< group id */
        int member_num;     /**< number of members in this group */
};

#endif // GROUP_H
