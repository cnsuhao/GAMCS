#include <pthread.h>
#include "../../SimGroup.h"
#include "Individual.h"

int main(void)
{
    SimGroup grp("group.conf");
    int num = grp.NumOfMembers();
    int i;
    pthread_t tids[num];
    Individual *indvs[num];

    for (i=0; i<num; i++)
    {
        printf("i: %d\n", i);
        indvs[0] = new Individual(i, &grp);
        tids[i] = indvs[0]->ThreadRun();
    }

    for (i=0; i<num; i++)
        pthread_join(tids[i], NULL);
}
