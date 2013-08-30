#ifndef GIOM_H
#define GIOM_H
#include <vector>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include "Debug.h"

typedef long Input;            /**< input identity */
typedef long Output;           /**< output identity  */

using namespace std;

class GIOM
{
    public:
        /** Default constructor */
        GIOM();
        /** Default destructor */
        virtual ~GIOM();
        Output Process(Input, vector<Output>);
        float Entropy();                                        /**< calculate entropy of this GIOM */
        void Update();
    protected:
        virtual vector<Output> Restrict(Input, vector<Output>);     /**< restrict the outputs */
        Input cur_in;
        Output cur_out;
    private:
};

#endif // GIOM_H
