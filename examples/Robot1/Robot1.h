#ifndef ROBOT1_H
#define ROBOT1_H
#include <signal.h>
#include "../../Entity.h"

class Robot1 : public Entity
{
    public:
        /** Default constructor */
        Robot1(int);
        /** Default destructor */
        virtual ~Robot1();

    protected:
    private:
        State position;

        State GetCurrentState();
        void DoAction(Action);
};

#endif // ROBOT1_H
