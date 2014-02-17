#include "Mouse.h"
#include "CSMMIAgent.h"
#include "Mysql.h"

int main(void)
{
    Mysql mysql;
    mysql.SetDBArgs("localhost", "root", "huangk", "Mouse");
    CSMMIAgent ma(1, 0.9, 0.01);
    ma.LoadMemoryFromStorage(&mysql);

    Mouse mouse("Mouse");
    mouse.ConnectAgent(&ma);
    mouse.Launch();

    ma.DumpMemoryToStorage(&mysql);
}
