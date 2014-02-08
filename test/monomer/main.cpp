#include "monomer_test.h"
#include "CSAgent.h"
#include "Mysql.h"

int main(void)
{
    Mysql mysql;
    mysql.SetDBArgs("localhost", "root", "huangk", "Monomer");
    CSAgent ma(0.9, 0.01);
    ma.SetStorage(&mysql);

    Monomer mouse(1);
    mouse.ConnectAgent(&ma);
    mouse.Launch();
}
