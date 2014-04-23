#include "monomer.h"
#include "gamcs/CSOSAgent.h"
#ifdef _MYSQL_FOUND_
#include "gamcs/Mysql.h"
#endif
#include "gamcs/CDotViewer.h"


int main(void)
{
    CSOSAgent ma(1, 0.9, 0.01);
#ifdef _MYSQL_FOUND_
    Mysql mysql("localhost", "root", "huangk", "Monomer");
    ma.loadMemoryFromStorage(&mysql);
#endif

    Monomer mono;
    mono.connectAgent(&ma);
    mono.stepLoop();

    //ma.deleteState(9);
    CDotViewer dotv;
    dotv.attachStorage(&ma);
    dotv.show();

#ifdef _MYSQL_FOUND_
    ma.dumpMemoryToStorage(&mysql);
#endif
}
