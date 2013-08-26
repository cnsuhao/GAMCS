#include "R1Agent.h"
#include "Robot1.h"

int main(void)
{
    R1Agent ra(15, 2, 0.8, 0.01);
    ra.SetDBArgs("localhost", "root", "890127", "MyAgent");
    ra.InitMemory();

    Robot1 rb1(1);
    rb1.SetAgent(&ra);
    rb1.Run();
}
