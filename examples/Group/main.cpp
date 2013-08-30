#include <pthread.h>
#include "../../SimGroup.h"
#include "../../SimAgent.h"
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
    SimAgent *sas[num];
    Individual *indvs[num];

    for (i=0; i<num; i++)
    {
        printf("i: %d\n", i);
        indvs[i] = new Individual(i);

        sas[i] = new SimAgent(0.8, 0.01);
        sprintf(str, "Indiv%d", i);
        sas[i]->SetDBArgs("localhost", "root", "890127", str);
        sas[i]->InitMemory();

        indvs[i]->ConnectAgent(sas[i]);
        indvs[i]->JoinGroup(&grp);
        tids[i] = indvs[i]->ThreadRun();
    }

    for(i=0; i<num; i++)
    {
        pthread_join(tids[i], NULL);
        delete indvs[i];
        delete sas[i];
    }

    return 0;
}
