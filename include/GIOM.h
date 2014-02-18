#ifndef GIOM_H
#define GIOM_H
#include <climits>    // LONG_MAX
#include <stddef.h>     // NULL
#include "Debug.h"

const unsigned long INVALID_INPUT = 0; /**< valid states start from 1 */
const long INVALID_OUTPUT = LONG_MAX; /**< the maximun value is used to indicate an invalid output, be careful! */

class OSpace;

/**
 * Generalized Input Output Model
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
        Output Process(Input, OSpace &); /**< choose an output value in an output space under a specified input value */
        float Entropy() const; /**< calculate entropy of this GIOM */
        virtual void Update(); /**< update inner states of GIOM, derived classes may have their own inner states to update */
    protected:
        virtual OSpace Restrict(Input, OSpace &) const; /**< restrict the output space to a subspace */
        Input cur_in; /**< input value */
        Output cur_out; /**< output value corresponding to cur_in */
        unsigned long process_count; /**< count of processing */
    private:
        long Random() const; /**< generate a random number in range 0 to LONG_MAX. It's where all possibilities and miracles come from! */
};

/**
 * Fragment to store a single output or a range of outputs.
 * For a singe output, the end is equal to start.
 */
struct OFragment
{
        GIOM::Output start; /**< the starting output value */
        GIOM::Output end; /**< the final output value */
        GIOM::Output step; /**< the increasing or decreasing step */
};

/**
 *  Output space.
 */
class OSpace
{
    public:
        enum
        {
            SPARE_CAPACITY = 16
        };

        typedef unsigned long olsize_t;

        explicit OSpace(olsize_t initfn = 0) :
                frag_num(initfn), the_capacity(initfn + SPARE_CAPACITY), current_index(
                        0), outputs(NULL)
        {
            outputs = new OFragment[the_capacity];
        }

        OSpace(const OSpace &other) :
                frag_num(0), the_capacity(SPARE_CAPACITY), current_index(0), outputs(
                NULL)
        {
            operator=(other);
        }

        virtual ~OSpace()
        {
            delete[] outputs;
        }

        /**
         * Check if the space is Empty
         * @return true or false
         */
        bool Empty() const
        {
            return (Size() == 0);
        }

        /**
         * Get the total number of outputs in space
         * @return  the number
         */
        olsize_t Size() const
        {
            olsize_t total_size = 0;
            // traverse each frament and collect the total Size
            for (olsize_t i = 0; i < frag_num; i++)
            {
                OFragment *frag = outputs + i;
                total_size += (frag->end - frag->start) / frag->step + 1;
            }

            return total_size;
        }

        /**
         * Current Capacity of the space
         * @return  Capacity
         */
        olsize_t Capacity() const
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
                OFragment *ptr = outputs + i;
                olsize_t out_num_in_this_frag = (ptr->end - ptr->start)
                        / ptr->step + 1;
                if (total_num + out_num_in_this_frag > index)    // index is in this fragment
                {
                    olsize_t index_in_frag = index - total_num;
                    out_wanted = ptr->start + ptr->step * index_in_frag;
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
        const OSpace &operator=(const OSpace &other)
        {
            if (this != &other)
            {
                delete[] outputs;
                // copy data
                frag_num = other.frag_num;
                the_capacity = other.the_capacity;

                outputs = new OFragment[the_capacity];
                // copy each fragment
                for (olsize_t i = 0; i < frag_num; i++)
                {
                    outputs[i] = other.outputs[i];
                }
            }

            return *this;
        }

        /**
         * Add a single output to space
         * @param output output
         */
        void Add(GIOM::Output output)
        {
            // check if exceeds the Capacity
            if (frag_num == the_capacity) Expand(2 * the_capacity + 1);

            OFragment new_frag;
            new_frag.start = output;
            new_frag.end = output;
            new_frag.step = 1;
            outputs[frag_num++] = new_frag;    // copy fragment and increase num
        }

        /**
         * Add a output range to space
         * @param start start output
         * @param end   end output
         * @param step increasing or decreasing step
         */
        void Add(GIOM::Output start, GIOM::Output end, GIOM::Output step)
        {
            // check range
            if ((end - start) / step < 0)
                ERROR("Invalid range! %ld --> %ld (step: %ld) \n", start,
                        end, step);

            if (frag_num == the_capacity) Expand(2 * the_capacity);

            OFragment new_frag;
            new_frag.start = start;
            new_frag.end = end;
            new_frag.step = step;
            outputs[frag_num++] = new_frag;
        }

        /**
         * Expend space Capacity.
         * @param ncap new Capacity
         */
        void Expand(olsize_t ncap)
        {
            if (ncap < frag_num) return;    // the new Capacity should at least include all current fragments

            OFragment *old_list = outputs;
            // Add up the spare
            ncap += SPARE_CAPACITY;
            outputs = new OFragment[ncap];
            // copy all fragments to new place
            for (olsize_t i = 0; i < frag_num; i++)
            {
                outputs[i] = old_list[i];
            }

            the_capacity = ncap;
            delete[] old_list;
        }

        /**
         * Clear the space.
         */
        void Clear()
        {
            frag_num = 0;
        }

        /**
         * Get the First output in space.
         * @return the First output
         */
        GIOM::Output First()
        {
            current_index = 0;
            return operator[](current_index);
        }

        /**
         * Get the Last output in space.
         * @return the Last output
         */
        GIOM::Output Last()
        {
            return operator[](Size());
        }

        /**
         * Iterator the space and get the Next output.
         * @return the Next output
         */
        GIOM::Output Next()
        {
            current_index++;
            return operator[](current_index);
        }

    private:
        olsize_t frag_num;    // number of fragments
        olsize_t the_capacity;    // space Capacity
        olsize_t current_index;    // used by iterator
        OFragment *outputs;
};
#endif // GIOM_H
