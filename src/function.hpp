#ifndef LASKIN_FUNCTION_HPP_GUARD
#define LASKIN_FUNCTION_HPP_GUARD

#include "signature.hpp"
#include "token.hpp"

namespace laskin
{
    class function
    {
    public:
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
                          void (*callback)(interpreter&, std::deque<value>&));

        /**
         * Constructs user defined function.
         */
        explicit function(const class signature& signature,
                          const std::vector<token>& callback);

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
            void (*n)(interpreter&, std::deque<value>&);
            std::vector<token>* c;
        } m_callback;
    };
}

#endif /* !LASKIN_FUNCTION_HPP_GUARD */
