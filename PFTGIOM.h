#ifndef PFGIOM_H
#define PFGIOM_H

#include "GIOM.h"

class PFTGIOM : public GIOM
{
    public:
        PFTGIOM(int, int);
        virtual ~PFTGIOM();
        virtual Output Process(Input);          // reimplement Process, add time sequence functionality to the process
    protected:
        virtual vector<Output> Restrict(Input, vector<Output>);     // reimplement Restrict
        /* bring in the time sequence feature */
        Input pre_in;           /**< previous input */
        Output pre_out;         /**< previous output */
    private:
};

#endif // PFGIOM_H
