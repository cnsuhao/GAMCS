#ifndef SIMGROUP_H
#define SIMGROUP_H
#include <pthread.h>
#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fstream>
#include "MyAgent.h"
#include "Entity.h"

using namespace std;

#define MAX_MEMBER 1000

struct Channel
{
    pthread_mutex_t mutex;
    struct Frame *frame;
};

struct Frame
{
    char data[2048];
    struct Frame *next;
};

struct Neigh
{
    int id;
    struct Neigh *next;
};

class SimGroup
{
    public:
        SimGroup(char *);
        virtual ~SimGroup();

        void SetTopo(char *);
        void Include(Entity *);
        void Run();
    protected:
        int Send(int, void *, size_t);
        int Recv(int, void *, size_t);

        int id;
        char *topofile;
        int member_num;

        void BuildNeighs();
        vector<int> GetNeighs(int);
        struct Channel *GetChannel(int);
        void Notify(int);

        struct Channel channels[MAX_MEMBER];
        struct Neigh *neighlist[MAX_MEMBER];

        vector<Entity *> entities;
        vector<pthread_t> tids;
    private:
};

#endif // SIMGROUP_H
