#include "monomer.h"
#include "gamcs/CSOSAgent.h"
#include "gamcs/Mysql.h"
#include "gamcs/CDotViewer.h"


int main(void)
{
    Mysql mysql;
    mysql.setDBArgs("localhost", "root", "huangk", "Monomer");
    CSOSAgent ma(1, 0.9, 0.01);
    ma.loadMemoryFromStorage(&mysql);

    Monomer mono("Monomer");
    mono.connectAgent(&ma);
    mono.stepLoop();

    //ma.deleteState(9);
    CDotViewer cdotv;
    cdotv.attachStorage(&ma);
    cdotv.show();

    ma.dumpMemoryToStorage(&mysql);
}
