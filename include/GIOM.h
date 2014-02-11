#ifndef GIOM_H
#define GIOM_H
#include <vector>
#include <climits>    // LONG_MAX

const unsigned long INVALID_INPUT = 0; /**< never use 0 for a valid state! */
const long INVALID_OUTPUT = LONG_MAX; /**< the maximun value is used to indicate invalidation, be careful! */

/**
 * General Input Output Model
 */
class GIOM
{
    public:
        typedef unsigned long Input; /**< input value, positive numbers */
        typedef long Output; /**< output value, output is the difference of two inputs, so it can be negetive, range: - max(Input) ~ +max(Input)*/

        /** Default constructor */
        GIOM();
        /** Default destructor */
        virtual ~GIOM();
        Output Process(Input, const std::vector<Output> &); /**< generate an output value from a output list given an input value */
        float Entropy(); /**< calculate entropy of this GIOM */
        virtual void Update(); /**< derived classes may update their own inner states */
    protected:
        virtual std::vector<Output> Restrict(Input,
                const std::vector<Output> &); /**< restrict the outputs */
        Input cur_in; /**< input value */
        Output cur_out; /**< output value corresponding to cur_in */
        unsigned long process_count;    /**< process count */
    private:
        long Random();  /**< generate a random number in range 0 to LONG_MAX. It's where all possibilities and miracles come from! */
};

#endif // GIOM_H
