#include "R1Agent.h"
#include "Robot1.h"

void signal_handler(int sig)
{
    if (sig == SIGINT)
        Robot1::quit = 1;
    return;
}

int main(void)
{
    signal(SIGINT, signal_handler);
    R1Agent ra(200, 2, 0.8, 0.01);
    ra.SetDBArgs("localhost", "root", "890127", "MyAgent");
    ra.InitMemory();

    Robot1 rb1(1);
    rb1.SetAgent(&ra);
    rb1.Run();
}
