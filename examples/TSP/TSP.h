#ifndef TSP_H
#define TSP_H

#include "TSPAgent.h"

class TSP
{
    public:
        TSP():ta(0, 0, 1, 0.01, "mem_file"), route(0x01) {}
        virtual ~TSP();

        void Run();
    protected:
    private:
        TSPAgent ta;
        State route;

        State GetCurrentState();
        void DoAction(Action);
};

#endif // TSP_H
