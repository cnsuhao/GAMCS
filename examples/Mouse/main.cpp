#include "Mouse.h"
#include "CSAgent.h"

void signal_handler(int sig)
{
    if (sig == SIGINT)
        Mouse1::quit = 1;
    return;
}

int main(void)
{
    signal(SIGINT, signal_handler);
    CSAgent ma(0.9, 0.01);
    ma.SetDBArgs("localhost", "root", "huangk", "Robot1");
    ma.InitMemory();

    Mouse1 rb1(1);
    rb1.ConnectAgent(&ma);
    rb1.Launch();
}
