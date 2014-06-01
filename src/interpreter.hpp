#ifndef LASKIN_INTERPRETER_HPP_GUARD
#define LASKIN_INTERPRETER_HPP_GUARD

#include "function.hpp"
#include "hashmap.hpp"
#include <vector>

namespace laskin
{
    class interpreter
    {
    public:
        interpreter();

        interpreter(const interpreter& that);

        /**
         * Initializes basic API. Should be called only once.
         */
        void initialize();

        /**
         * Executes sequence of tokens with given data stack.
         */
        void execute(const std::vector<token>& tokens,
                     class stack<value>& stack)
            throw(script_error, syntax_error);

        void register_function(const std::string& name,
                               const class signature& signature,
                               void (*callback)(interpreter&, stack<value>&));

        void register_function(const std::string& name,
                               const std::string& signature,
                               void (*callback)(interpreter&, stack<value>&));

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
        hashmap<std::vector<function> > m_functions;
    };
}

#endif /* !LASKIN_INTERPRETER_HPP_GUARD */
