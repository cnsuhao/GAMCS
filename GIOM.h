#ifndef GIOM_H
#define GIOM_H
#include <vector>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <stdio.h>

// debug on/off
#ifdef _DEBUG_
#define dbgprt(fmt,...)  do{printf("DBG-PRINT:");printf(fmt,##__VA_ARGS__);}while(0)
#else
#define dbgprt(fmt,...)
#endif

#define ERROR(fmt, ...) do{printf("ERROR:");printf(fmt, ##__VA_ARGS__);exit(-1);}while(0)

typedef unsigned long Input;            /**< input identity */
typedef unsigned long Output;           /**< output identity  */

using namespace std;

class GIOM
{
    public:
        /** Default constructor */
        GIOM(int, int);
        /** Default destructor */
        virtual ~GIOM();
        virtual Output Process(Input);
    protected:
        virtual vector<Output> OutList(Input);                      /**< get the outputs in correspond with specfic input */
        virtual vector<Output> Restrict(Input, vector<Output>);     /**< restrict the outputs */
        float RectEntropy();                                        /**< calculate entropy of this GIOM */
        /* I:N/O:M */
        int N;                                                      /**< number of inputs */
        int M;                                                      /**< number of outputs of one input */
    private:
};

#endif // GIOM_H
