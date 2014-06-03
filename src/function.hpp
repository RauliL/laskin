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
            type_script
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
                          const class script& script);

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
                    stack<value>& data,
                    hashmap<value>& locals,
                    std::istream& in,
                    std::ostream& out) const
            throw(script_error, syntax_error);

        function& assign(const function& that);
        function& assign(const class signature& signature,
                         callback n);
        function& assign(const class signature& signature,
                         const class script& script);

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
            script* s;
        } m_callback;
    };
}

#endif /* !LASKIN_FUNCTION_HPP_GUARD */
