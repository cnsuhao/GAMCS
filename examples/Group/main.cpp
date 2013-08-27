#include <pthread.h>
#include "../../SimGroup.h"
#include "Individual.h"

void signal_handler(int sig)
{
    if (sig == SIGINT)
    {
        Individual::quit = 1;
    }
    return;
}

int main(void)
{
    char str[128];

    signal(SIGINT, signal_handler);

    SimGroup grp(1);
    grp.LoadTopo("group.conf");

    int num = grp.NumOfMembers();
    int i;
    pthread_t tids[num];
    R1Agent *ras[num];
    Individual *indvs[num];

    for (i=0; i<num; i++)
    {
        printf("i: %d\n", i);
        indvs[i] = new Individual(i);

        ras[i] = new R1Agent(15, 2, 0.8, 0.01);
        sprintf(str, "MyAgent%d", i);
        ras[i]->SetDBArgs("localhost", "root", "890127", str);
        ras[i]->InitMemory();

        indvs[i]->SetAgent(ras[i]);
        indvs[i]->SetGroup(&grp);
        tids[i] = indvs[i]->ThreadRun();
    }

    for(i=0; i<num; i++)
    {
        pthread_join(tids[i], NULL);
        delete ras[i];
    }

    return 0;
}
