#include "monomer.h"
#include "CSAgent.h"
#include "Mysql.h"

int main(void)
{
    Mysql mysql;
    mysql.SetDBArgs("localhost", "root", "huangk", "Monomer");
    CSAgent ma(1, 0.9, 0.01);
    ma.SetDegreeOfCuriosity(100.0);
    ma.LoadMemoryFromStorage(&mysql);

    Monomer mono("Monomer");
    mono.ConnectAgent(&ma);
    mono.Launch();

    ma.DumpMemoryToStorage(&mysql);
}
