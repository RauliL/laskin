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
    
    class rational
    {
    public:
        rational(integer numerator, integer denominator);
        rational(const rational& that);

        inline integer numerator() const
        {
            return m_numerator;
        }

        inline integer denominator() const
        {
            return m_denominator;
        }

        rational& assign(const rational& that);
        rational& assign(integer numerator, integer denominator);

        /**
         * Assignment operator.
         */
        inline rational& operator=(const rational& that)
        {
            return assign(that);
        }

    private:
        integer m_numerator;
        integer m_denominator;
    };
}

#endif /* !LASKIN_NUMBERS_HPP_GUARD */
