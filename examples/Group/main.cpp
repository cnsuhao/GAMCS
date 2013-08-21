#include <pthread.h>
#include "../../SimGroup.h"
#include "Individual.h"

int main(void)
{
    SimGroup grp("group.conf");
    int num = grp.NumOfMembers();
    int i;
    pthread_t tids[num];
    for (i=0; i<num; i++)
    {
        printf("i: %d\n", i);
        Individual indv(i);
        tids[i] = indv.ThreadRun();
    }
    printf("exit------------------\n");
    sleep(10);
}
