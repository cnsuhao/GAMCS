#include "Mouse.h"
#include "CSAgent.h"
#include "Mysql.h"

void signal_handler(int sig)
{
    if (sig == SIGINT)
    {
        // do something
    }
    return;
}

int main(void)
{
    signal(SIGINT, signal_handler);
    Mysql mysql;
    mysql.SetDBArgs("localhost", "root", "huangk", "Mouse");
    CSAgent ma(0.9, 0.01);
    ma.SetStorage(&mysql);
    ma.InitMemory();

    Mouse mouse(1);
    mouse.ConnectAgent(&ma);
    mouse.Launch();
}
