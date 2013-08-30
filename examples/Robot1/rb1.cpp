#include "Robot1.h"
#include "../../SimAgent.h"

void signal_handler(int sig)
{
    if (sig == SIGINT)
        Robot1::quit = 1;
    return;
}

int main(void)
{
    signal(SIGINT, signal_handler);
    SimAgent ma(0.9, 0.01);
    ma.SetDBArgs("localhost", "root", "890127", "Robot1");
    ma.InitMemory();

    Robot1 rb1(1);
    rb1.ConnectAgent(&ma);
    rb1.Run();
}
