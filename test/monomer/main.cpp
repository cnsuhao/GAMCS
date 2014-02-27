#include "monomer.h"
#include "gimcs/CSMAgent.h"
#include "gimcs/Mysql.h"

int main(void)
{
    Mysql mysql;
    mysql.SetDBArgs("localhost", "root", "huangk", "Monomer");
    CSMAgent ma(1, 0.5, 0.11);
    ma.SetDegreeOfCuriosity(0.0);
    ma.LoadMemoryFromStorage(&mysql);

    Monomer mono("Monomer");
    mono.ConnectAgent(&ma);
    mono.Launch();

    ma.DumpMemoryToStorage(&mysql);
}
