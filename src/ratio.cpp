#include "ratio.hpp"

namespace laskin
{
    ratio::ratio(integer numerator, integer denominator)
        throw(std::domain_error)
        : m_numerator(numerator)
        , m_denominator(denominator == 0 ? 1 : denominator)
    {
        if (denominator == 0)
        {
            throw std::domain_error("division by zero");
        }
    }

    ratio::ratio(const ratio& that)
        : m_numerator(that.m_numerator)
        , m_denominator(that.m_denominator) {}

    ratio ratio::negate() const
    {
        return ratio(-m_numerator, m_denominator);
    }

    bool ratio::equals(const ratio& that) const
    {
        return m_numerator == that.m_numerator
            && m_denominator == that.m_denominator;
    }

    ratio& ratio::assign(const ratio& that)
    {
        m_numerator = that.m_numerator;
        m_denominator = that.m_denominator;

        return *this;
    }

    std::ostream& operator<<(std::ostream& os, const class ratio& ratio)
    {
        os << int_to_string(ratio.numerator())
           << '/'
           << int_to_string(ratio.denominator());

        return os;
    }
}
