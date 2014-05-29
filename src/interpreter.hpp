#ifndef LASKIN_INTERPRETER_HPP_GUARD
#define LASKIN_INTERPRETER_HPP_GUARD

#include "error.hpp"

namespace laskin
{
    class interpreter
    {
    public:
        interpreter();

        interpreter(const interpreter& that);

        interpreter& assign(const interpreter& that);

        /**
         * Assignment operator.
         */
        inline interpreter& operator=(const interpreter& that)
        {
            return assign(that);
        }
    };
}

#endif /* !LASKIN_INTERPRETER_HPP_GUARD */
