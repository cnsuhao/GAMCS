#ifndef GIOM_H
#define GIOM_H
#include <climits>    // LONG_MAX
#include <stddef.h>
#include "Debug.h"

const unsigned long INVALID_INPUT = 0; /**< never use 0 for a valid state! */
const long INVALID_OUTPUT = LONG_MAX; /**< the maximun value is used to indicate invalidation, be careful! */

class OutList;

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
        Output Process(Input, OutList &); /**< generate an output value from a output list given an input value */
        float Entropy(); /**< calculate entropy of this GIOM */
        virtual void Update(); /**< derived classes may update their own inner states */
    protected:
        virtual OutList Restrict(Input, OutList &); /**< restrict the outputs */
        Input cur_in; /**< input value */
        Output cur_out; /**< output value corresponding to cur_in */
        unsigned long process_count; /**< process count */
    private:
        long Random(); /**< generate a random number in range 0 to LONG_MAX. It's where all possibilities and miracles come from! */
};

/**
 * Fragment to store a single output or a range of outputs.
 * For a singe output, the end is equal to start.
 */
struct OutFragment
{
        GIOM::Output start;
        GIOM::Output end;
        GIOM::Output interval;
};

/**
 *  List for storing outputs.
 */
class OutList
{
    public:
        enum
        {
            SPARE_CAPACITY = 16
        };

        typedef unsigned long olsize_t;

        explicit OutList(olsize_t initfn = 0) :
                frag_num(initfn), the_capacity(initfn + SPARE_CAPACITY), current_index(
                        0), outputs(NULL)
        {
            outputs = new OutFragment[the_capacity];
        }

        OutList(const OutList &other) :
                frag_num(0), the_capacity(SPARE_CAPACITY), current_index(0), outputs(
                NULL)
        {
            operator=(other);
        }

        virtual ~OutList()
        {
            delete[] outputs;
        }

        /**
         * Check if the outlist is empty
         * @return true or false
         */
        bool empty() const
        {
            return (size() == 0);
        }

        /**
         * Get the total number of outputs in the list
         * @return  the number
         */
        olsize_t size() const
        {
            olsize_t total_size = 0;
            // traverse each frament and collect the total size
            for (olsize_t i = 0; i < frag_num; i++)
            {
                OutFragment *frag = outputs + i;
                total_size += (frag->end - frag->start) / frag->interval + 1;
            }

            return total_size;
        }

        /**
         * Current capacity of the list
         * @return  capacity
         */
        olsize_t capacity() const
        {
            return the_capacity;
        }

        /**
         * Override operator [].
         * For index out of bound returns INVALID_OUTPUT
         * @param index output index
         * @return output value
         */
        GIOM::Output operator[](olsize_t index) const
        {
            olsize_t total_num = 0;
            olsize_t out_wanted;

            if (index < total_num)    // subscript out of bound
            {
                return INVALID_OUTPUT;
            }

            olsize_t i;
            // traverse each fragment to find which fragment the index locates in
            for (i = 0; i < frag_num; i++)
            {
                OutFragment *ptr = outputs + i;
                olsize_t out_num_in_this_frag = (ptr->end - ptr->start)
                        / ptr->interval + 1;
                if (total_num + out_num_in_this_frag > index)    // index is in this fragment
                {
                    olsize_t index_in_frag = index - total_num;
                    out_wanted = ptr->start + ptr->interval * index_in_frag;
                    break;
                }

                total_num += out_num_in_this_frag;
            }

            if (i >= frag_num)    // superscript out of bound
            {
                return INVALID_OUTPUT;
            }

            return out_wanted;
        }

        /**
         * Override operator =
         * @param other another OutList object
         * @return  reassigned object
         */
        const OutList &operator=(const OutList &other)
        {
            if (this != &other)
            {
                delete[] outputs;
                // copy data
                frag_num = other.frag_num;
                the_capacity = other.the_capacity;

                outputs = new OutFragment[the_capacity];
                // copy each fragment
                for (olsize_t i = 0; i < frag_num; i++)
                {
                    outputs[i] = other.outputs[i];
                }
            }

            return *this;
        }

        /**
         * Add a single output to list
         * @param output output
         */
        void add(GIOM::Output output)
        {
            // check if exceeds the capacity
            if (frag_num == the_capacity) expand(2 * the_capacity + 1);

            OutFragment new_frag;
            new_frag.start = output;
            new_frag.end = output;
            new_frag.interval = 1;
            outputs[frag_num++] = new_frag;    // copy fragment and increase num
        }

        /**
         * Add a output range to list
         * @param start start output
         * @param end   end output
         * @param interval increasing or decreasing interval
         */
        void add(GIOM::Output start, GIOM::Output end, GIOM::Output interval)
        {
            // check range
            if ((end - start) / interval < 0)
                ERROR("Invalid range! %ld --> %ld (interval: %ld) \n", start, end, interval);

            if (frag_num == the_capacity) expand(2 * the_capacity);

            OutFragment new_frag;
            new_frag.start = start;
            new_frag.end = end;
            new_frag.interval = interval;
            outputs[frag_num++] = new_frag;
        }

        /**
         * Expend capacity for future adding
         * @param ncap new capacity
         */
        void expand(olsize_t ncap)
        {
            if (ncap < frag_num) return;    // the new capacity should at least include all current fragments

            OutFragment *old_list = outputs;
            // add up the spare
            ncap += SPARE_CAPACITY;
            outputs = new OutFragment[ncap];
            // copy all fragments to new place
            for (olsize_t i = 0; i < frag_num; i++)
            {
                outputs[i] = old_list[i];
            }

            the_capacity = ncap;
            delete[] old_list;
        }

        /**
         * Clear the list.
         */
        void clear()
        {
            frag_num = 0;
        }

        /**
         * Get the first output in list.
         * @return the first output
         */
        GIOM::Output first()
        {
            current_index = 0;
            return operator[](current_index);
        }

        /**
         * Get the last output in list.
         * @return the last output
         */
        GIOM::Output last()
        {
            return operator[](size());
        }

        /**
         * Iterator list and get the next output.
         * @return the next output
         */
        GIOM::Output next()
        {
            current_index++;
            return operator[](current_index);
        }

    private:
        olsize_t frag_num;    // number of fragments
        olsize_t the_capacity;    // list capacity
        olsize_t current_index;    // used by iterator
        OutFragment *outputs;
};
#endif // GIOM_H
