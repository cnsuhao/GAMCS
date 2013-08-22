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

    sleep(20);
//    for (i=0; i<num; i++)
//    {
//        printf("tid%d: %d\n", i, tids[i]);
//        pthread_join(tids[i], NULL);
//    }
}
