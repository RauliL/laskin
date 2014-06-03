#include "ratio.hpp"

namespace laskin
{
    ratio::ratio()
        : m_numerator(0)
        , m_denominator(1) {}

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

    ratio ratio::add(const ratio& that) const
    {
        return ratio(
                m_numerator * that.m_denominator + m_denominator * that.m_numerator,
                m_denominator * that.m_denominator
        );
    }

    ratio ratio::substract(const ratio& that) const
    {
        return ratio(
                m_numerator * that.m_denominator - m_denominator * that.m_numerator,
                m_denominator * that.m_denominator
        );
    }

    ratio ratio::multiply(const ratio& that) const
    {
        return ratio(
                m_numerator * that.m_numerator,
                m_denominator * that.m_denominator
        );
    }

    ratio ratio::divide(const ratio& that) const
    {
        return ratio(
                m_numerator * that.m_denominator,
                m_denominator * that.m_numerator
        );
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
