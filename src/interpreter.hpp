#ifndef LASKIN_INTERPRETER_HPP_GUARD
#define LASKIN_INTERPRETER_HPP_GUARD

#include "function.hpp"
#include "hashmap.hpp"
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

        void register_function(const std::string& name,
                               const class signature& signature,
                               void (*callback)(interpreter&, std::deque<value>&));

        void register_function(const std::string& name,
                               const class signature& signature,
                               const std::vector<token>& callback);

        interpreter& assign(const interpreter& that);

        /**
         * Assignment operator.
         */
        inline interpreter& operator=(const interpreter& that)
        {
            return assign(that);
        }

    private:
        /** Container for functions. */
        hashmap<function> m_functions;
    };
}

#endif /* !LASKIN_INTERPRETER_HPP_GUARD */
