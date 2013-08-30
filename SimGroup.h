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
#include <assert.h>
#include "Group.h"
#include "Debug.h"

using namespace std;

#define MAX_MEMBER 1000
#define CHANNEL_SIZE 10
#define DATA_SIZE 2048

struct Msg
{
    int sender_id;
    char data[DATA_SIZE];
};

struct Channel
{
    pthread_mutex_t mutex;
    int ptr;
    int msg_num;
    struct Msg *msg;
};

struct Neigh
{
    int id;
    struct Neigh *next;
};

class SimGroup : public Group
{
    public:
        SimGroup(int);
        virtual ~SimGroup();

        void LoadTopo(string);
        int NumOfMembers();

        int Send(int, void *, size_t);
        int Recv(int, void *, size_t);
    protected:
    private:
        string topofile;

        void BuildNeighsChannels();
        vector<int> GetNeighs(int);
        struct Channel *GetChannel(int);
        void Notify(int);

        vector<int> nodes;
        struct Channel channels[MAX_MEMBER];
        struct Neigh *neighlist[MAX_MEMBER];
};

#endif // SIMGROUP_H
