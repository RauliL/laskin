#ifndef LASKIN_INTERPRETER_HPP_GUARD
#define LASKIN_INTERPRETER_HPP_GUARD

#include "function.hpp"
#include <vector>

namespace laskin
{
    class interpreter
    {
    public:
        typedef hashmap<std::vector<function> > function_map;

        interpreter();

        interpreter(const interpreter& that);

        /**
         * Initializes basic API. Should be called only once.
         */
        void initialize();

        void execute(const std::string& word,
                     stack<value>& data,
                     hashmap<value>& locals,
                     std::istream& in,
                     std::ostream& out)
            throw(error);

        void register_function(const std::string& name,
                               const class signature& signature,
                               function::callback callback);

        void register_function(const std::string& name,
                               const std::string& signature,
                               function::callback callback);

        void register_function(const std::string& name,
                               const class signature& signature,
                               const class script& script);

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
        function_map m_functions;
    };
}

#endif /* !LASKIN_INTERPRETER_HPP_GUARD */
