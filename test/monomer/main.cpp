#include "monomer.h"
#include "gimcs/CSMAgent.h"
#include "gimcs/Mysql.h"

int main(void)
{
    Mysql mysql;
    mysql.setDBArgs("localhost", "root", "huangk", "Monomer");
    CSMAgent ma(1, 0.9, 0.01);
    ma.loadMemoryFromStorage(&mysql);

    Monomer mono("Monomer");
    mono.connectAgent(&ma);
    mono.launch();

    ma.deleteState(6);
    ma.deleteState(9);

    ma.dumpMemoryToStorage(&mysql);
}
