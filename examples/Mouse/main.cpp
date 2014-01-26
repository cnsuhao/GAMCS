#include "Mouse.h"
#include "CSAgent.h"
#include "Mysql.h"

int main(void)
{
    Mysql mysql;
    mysql.SetDBArgs("localhost", "root", "huangk", "Mouse");
    CSAgent ma(0.9, 0.01);
    ma.SetStorage(&mysql);

    Mouse mouse(1);
    mouse.ConnectAgent(&ma);
    mouse.Launch();
}
