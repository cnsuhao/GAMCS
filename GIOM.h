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

typedef unsigned long Input;            /**< input value, positive numbers */
typedef unsigned long Output;           /**< output value  */

const unsigned long INVALID_VALUE = 0;           /**< 0 means invalid input or output value */

using namespace std;

/**
* General Input Output Model
*/
class GIOM
{
    public:
        /** Default constructor */
        GIOM();
        /** Default destructor */
        virtual ~GIOM();
        Output Process(Input, vector<Output>);          /**< generate an output value from a output list given an input value */
        float Entropy();                                        /**< calculate entropy of this GIOM */
        void Update();
    protected:
        virtual vector<Output> Restrict(Input, vector<Output>);     /**< restrict the outputs */
        Input cur_in;       /**< input value */
        Output cur_out;     /**< output value corresponding to cur_in */
    private:
};

#endif // GIOM_H
