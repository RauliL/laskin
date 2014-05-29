#ifndef LASKIN_INTERPRETER_HPP_GUARD
#define LASKIN_INTERPRETER_HPP_GUARD

#include "defines.hpp"
#include "error.hpp"
#include <deque>
#include <vector>

namespace laskin
{
    class interpreter
    {
    public:
        interpreter();

        interpreter(const interpreter& that);

        /**
         * Executes sequence of tokens with given data stack.
         */
        void execute(const std::vector<token>& tokens,
                     std::deque<value>& stack)
            throw(script_error, syntax_error);

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
