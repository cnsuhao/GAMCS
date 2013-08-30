#ifndef PFGIOM_H
#define PFGIOM_H

#include "GIOM.h"

class PFTGIOM : public GIOM
{
    public:
        PFTGIOM();
        virtual ~PFTGIOM();
        Output Process(Input, vector<Output>);          // reimplement Process, add time sequence functionality to the process
        void Update();
    protected:
        virtual vector<Output> Restrict(Input, vector<Output>);     // reimplement Restrict
        /* bring in the time sequence feature */
        Input pre_in;           /**< previous input */
        Output pre_out;         /**< previous output */
    private:
};

#endif // PFGIOM_H
