#include "Mouse.h"
#include "CSAgent.h"

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
    CSAgent ma(0.9, 0.01);
    ma.SetDBArgs("localhost", "root", "huangk", "Mouse");
    ma.InitMemory();

    Mouse mouse(1);
    mouse.ConnectAgent(&ma);
    mouse.Launch();
}
