#include "ratio.hpp"

namespace laskin
{
    static inline integer gcd(integer a, integer b)
    {
        if (b == 0)
        {
            return a;
        } else {
            return gcd(b, a % b);
        }
    }

    ratio::ratio()
        : m_numerator(0)
        , m_denominator(1) {}

    ratio::ratio(integer numerator, integer denominator)
        throw(std::domain_error)
        : m_numerator(0)
        , m_denominator(1)
    {
        if (denominator == 0)
        {
            throw std::domain_error("division by zero");
        } else {
            const integer g = gcd(numerator, denominator);

            m_numerator = numerator / g;
            m_denominator = denominator / g;
        }
    }

    ratio::ratio(const ratio& that)
        : m_numerator(that.m_numerator)
        , m_denominator(that.m_denominator) {}

    ratio ratio::negate() const
    {
        return ratio(-m_numerator, m_denominator);
    }

    ratio ratio::add(const ratio& that) const
    {
        return ratio(
                m_numerator * that.m_denominator + m_denominator * that.m_numerator,
                m_denominator * that.m_denominator
        );
    }

    ratio ratio::add(integer that) const
    {
        return add(ratio(that));
    }

    ratio ratio::substract(const ratio& that) const
    {
        return ratio(
                m_numerator * that.m_denominator - m_denominator * that.m_numerator,
                m_denominator * that.m_denominator
        );
    }

    ratio ratio::substract(integer that) const
    {
        return substract(ratio(that));
    }

    ratio ratio::multiply(const ratio& that) const
    {
        return ratio(
                m_numerator * that.m_numerator,
                m_denominator * that.m_denominator
        );
    }

    ratio ratio::multiply(integer that) const
    {
        return multiply(ratio(that));
    }

    ratio ratio::divide(const ratio& that) const
    {
        return ratio(
                m_numerator * that.m_denominator,
                m_denominator * that.m_numerator
        );
    }

    ratio ratio::divide(integer that) const
    {
        return divide(ratio(that));
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
