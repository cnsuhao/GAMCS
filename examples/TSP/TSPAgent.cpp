/**********************************************************************
*	@File:
*	@Created: 2013-8-20
*	@Author: AndyHuang - andyspider@126.coom
*	@Description:
*
*	@Modify date:
***********************************************************************/
#include "TSPAgent.h"

TSPAgent::TSPAgent(int n, int m, float dr, float th, string mf):MyAgent(n, m, dr, th, mf)
{
    //ctor
    city_num = LoadCities();
    Init();
}

TSPAgent::~TSPAgent()
{
    //dtor
}

void TSPAgent::Init()
{
    un_cities.clear();
    for(int i=2; i< city_num + 1; i++)      // 去除1号本身
    {
        un_cities.push_back(i);
    }
    ex_cities.clear();

    pre_in = -1;
}

vector<Action> TSPAgent::OutList(State st)
{
        // 删除城市
    if (pre_out == un_cities.back())
    {
        un_cities.pop_back();
    }
    else
    {
        for (vector<int>::iterator it = un_cities.begin();
        it != un_cities.end(); ++it)
        {
            if (*it == pre_out)
            {
                un_cities.erase(it);
            }
        }

    }

    ex_cities.push_back(pre_out);                // 一开始的-1也会压进来

    if (ex_cities.size() == city_num)            // 其他城市都经历过之后，出现最开始的城市
        un_cities.push_back(1);

    vector<Action> acts;
    acts.clear();
    for (vector<int>::iterator it = un_cities.begin();
    it != un_cities.end(); ++it)
        acts.push_back((Action) *it);

    return acts;
}

float TSPAgent::OriginalPayoff(State st)
{
    State tmp = st;
    tmp &= 0xffff;
    int city1_num = (tmp >> 8);
    int city2_num = (tmp & 0xff);
    printf("city1: %d, city2: %d\n", city1_num, city2_num);
    if (city1_num == 0)         // 初始态
        return 0;
    struct City city1 = cities[city1_num - 1];
    struct City city2 = cities[city2_num - 1];

    int xd = city2.xpos - city1.xpos;
    int yd = city2.ypos - city1.ypos;
    float rij = sqrt((xd*xd + yd*yd)/ 10.0);
    int tij = round(rij);
    int dij;
    if (tij < rij) dij = tij + 1;
    else dij = tij;

    return -dij;
}

State TSPAgent::ActionEffect(State st, Action act)
{
    st  &= 0xff;
    st <<= 8;
    st |= act;
    return st;
}

int TSPAgent::LoadCities()
{
    ifstream df("att48.data");
    cities.clear();
    struct City ct;
    int xpos, ypos;
    int num = 0;

    while (!df.eof())
    {

        df >> xpos >> ypos;

        ct.xpos = xpos;
        ct.ypos = ypos;
        cities.push_back(ct);
        num++;
    }
    df.close();

    return num;
}
