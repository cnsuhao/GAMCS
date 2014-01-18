#ifndef PFGIOM_H
#define PFGIOM_H

#include "GIOM.h"

/**
* Partial Feedback & Time Sequence General Input Output Model
*/
class PFTGIOM : public GIOM
{
    public:
        PFTGIOM();
        virtual ~PFTGIOM();
        Output Process(Input, const std::vector<Output> &);          // reimplement Process, add time sequence functionality to the process
        void Update();
    protected:
        virtual std::vector<Output> Restrict(Input, const std::vector<Output> &);     // reimplement Restrict
        /* bring in the time sequence feature */
        Input pre_in;           /**< previous input value */
        Output pre_out;         /**< previous output value */
    private:
};

#endif // PFGIOM_H
