#ifndef TSGIOM_H
#define TSGIOM_H

#include "GIOM.h"

/**
* Time Sequence General Input Output Model
*/
class TSGIOM : public GIOM
{
    public:
        TSGIOM();
        virtual ~TSGIOM();
        Output Process(Input, const std::vector<Output> &);          // reimplement Process, add time sequence functionality to the process
        void Update();
    protected:
        virtual std::vector<Output> Restrict(Input, const std::vector<Output> &);     // reimplement Restrict
        /* bring in the time sequence feature */
        Input pre_in;           /**< previous input value */
        Output pre_out;         /**< previous output value */
    private:
};

#endif // TSGIOM_H
