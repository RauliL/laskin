#ifndef LASKIN_FUNCTION_HPP_GUARD
#define LASKIN_FUNCTION_HPP_GUARD

#include "hashmap.hpp"
#include "signature.hpp"
#include "token.hpp"

namespace laskin
{
    class function
    {
    public:
        typedef void (*callback)(interpreter&,
                                 stack<value>&,
                                 hashmap<value>&,
                                 std::istream&,
                                 std::ostream&);

        enum type
        {
            type_native,
            type_custom
        };

        function();

        /**
         * Constructs native function.
         */
        explicit function(const class signature& signature,
                          callback n);

        /**
         * Constructs user defined function.
         */
        explicit function(const class signature& signature,
                          const std::vector<token>& c);

        function(const function& that);

        virtual ~function();

        /**
         * Returns type of the function.
         */
        inline enum type type() const
        {
            return m_type;
        }

        /**
         * Returns function signature.
         */
        inline const class signature& signature() const
        {
            return m_signature;
        }

        void invoke(class interpreter& interpreter,
                    class stack<value>& stack,
                    hashmap<value>& local_variables,
                    std::istream& in,
                    std::ostream& out) const
            throw(script_error, syntax_error);

        function& assign(const function& that);

        /**
         * Assignment operator.
         */
        inline function& operator=(const function& that)
        {
            return assign(that);
        }

    private:
        /** Type of the function. */
        enum type m_type;
        /** Function signature. */
        class signature m_signature;
        union
        {
            callback n;
            std::vector<token>* c;
        } m_callback;
    };
}

#endif /* !LASKIN_FUNCTION_HPP_GUARD */
