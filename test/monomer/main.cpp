#include "monomer.h"
#include "CSMMIAgent.h"
#include "Mysql.h"

int main(void)
{
    Mysql mysql;
    mysql.SetDBArgs("localhost", "root", "huangk", "Monomer");
    CSMMIAgent ma(1, 0.9, 0.01);
    ma.SetDegreeOfCuriosity(0.0);
    ma.LoadMemoryFromStorage(&mysql);

    Monomer mono("Monomer");
    mono.ConnectAgent(&ma);
    mono.Launch();

    ma.DumpMemoryToStorage(&mysql);
}
