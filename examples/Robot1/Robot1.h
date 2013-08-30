#ifndef ROBOT1_H
#define ROBOT1_H
#include <signal.h>
#include "../../Avatar.h"

class Robot1 : public Avatar
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
        vector<Action> ActionList(State);
        State ExpectedState();
        float OriginalPayoff(State);
};

#endif // ROBOT1_H
