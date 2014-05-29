#ifndef LASKIN_FUNCTION_HPP_GUARD
#define LASKIN_FUNCTION_HPP_GUARD

#include "defines.hpp"
#include "token.hpp"
#include <deque>

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

        function(const function& that);

        virtual ~function();

        /**
         * Returns type of the function.
         */
        inline enum type type() const
        {
            return m_type;
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
        enum type m_type;
        union
        {
            void (*n)(interpreter&, std::deque<value>&);
            std::vector<token>* c;
        } m_callback;
    };
}

#endif /* !LASKIN_FUNCTION_HPP_GUARD */
