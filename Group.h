#ifndef GROUP_H
#define GROUP_H
#include <stdlib.h>

class Group
{
    public:
        Group();
        virtual ~Group();

        virtual int Send(int, void *, size_t) = 0;
        virtual int Recv(int, void *, size_t) = 0;
    protected:
        int id;
        int member_num;
    private:
};

#endif // GROUP_H
