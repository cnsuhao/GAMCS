#ifndef TSPAGENT_H
#define TSPAGENT_H

#include <fstream>
#include "../../MyAgent.h"

struct City
{
    int xpos;
    int ypos;
};

class TSPAgent : public MyAgent
{
    public:
        TSPAgent(int, int, float, float, string);
        virtual ~TSPAgent();
        void Init();
    protected:
        virtual vector<Action> OutList(State);
        virtual float OriginalPayoff(State);
        virtual State ActionEffect(State, Action);
    private:
        int LoadCities();

        string datafile;
        vector<struct City> cities;
        vector<int> ex_cities;
        vector<int> un_cities;
        int city_num;
};

#endif // TSPAGENT_H
