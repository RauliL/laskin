#ifndef LASKIN_NUMBERS_HPP_GUARD
#define LASKIN_NUMBERS_HPP_GUARD

#include "defines.hpp"
#include <stdexcept>
#include <string>

namespace laskin
{
    std::string int_to_string(integer, int = 10);
    std::string real_to_string(real);

    integer string_to_int(const std::string&, int = -1)
        throw(std::length_error);
    real string_to_real(const std::string&)
        throw(std::length_error);
    
    class ratio
    {
    public:
        ratio(integer numerator = 0, integer denominator = 1);
        ratio(const ratio& that);

        inline integer numerator() const
        {
            return m_numerator;
        }

        inline integer denominator() const
        {
            return m_denominator;
        }

        ratio& assign(const ratio& that);
        ratio& assign(integer numerator, integer denominator);

        /**
         * Assignment operator.
         */
        inline ratio& operator=(const ratio& that)
        {
            return assign(that);
        }

    private:
        integer m_numerator;
        integer m_denominator;
    };
}

#endif /* !LASKIN_NUMBERS_HPP_GUARD */
