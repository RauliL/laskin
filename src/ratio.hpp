#ifndef LASKIN_RATIO_HPP_GUARD
#define LASKIN_RATIO_HPP_GUARD

#include "numbers.hpp"
#include <iostream>

namespace laskin
{
    /**
     * Implementation of rational number.
     */
    class ratio
    {
    public:
        /**
         * Constructs rational number which represents zero.
         */
        ratio();

        /**
         * Constructs new rational number.
         *
         * \param numerator         Numerator part
         * \param denominator       Denominator part
         * \throw std::domain_error If denominator is zero
         */
        explicit ratio(integer numerator, integer denominator = 1)
            throw(std::domain_error);

        /**
         * Copy constructor.
         *
         * \param that Existing ratio to construct copy of
         */
        ratio(const ratio& that);

        /**
         * Returns denominator.
         */
        inline integer denominator() const
        {
            return m_denominator;
        }

        /**
         * Returns numerator.
         */
        inline integer numerator() const
        {
            return m_numerator;
        }

        ratio negate() const;

        inline ratio operator-() const
        {
            return negate();
        }

        ratio add(const ratio& that) const;

        inline ratio operator+(const ratio& that) const
        {
            return add(that);
        }

        ratio substract(const ratio& that) const;

        inline ratio operator-(const ratio& that) const
        {
            return substract(that);
        }

        ratio multiply(const ratio& that) const;

        inline ratio operator*(const ratio& that) const
        {
            return multiply(that);
        }

        /**
         * Tests whether two rational numbers are equal.
         *
         * \param that Other rational number to test equality with
         */
        bool equals(const ratio& that) const;

        /**
         * Equality testing operator.
         */
        inline bool operator==(const ratio& that) const
        {
            return equals(that);
        }

        /**
         * Non-equality testing operator.
         */
        inline bool operator!=(const ratio& that) const
        {
            return !equals(that);
        }

        ratio& assign(const ratio& that);

        /**
         * Assignment operator.
         */
        inline ratio& operator=(const ratio& that)
        {
            return assign(that);
        }

    private:
        /** The numerator. */
        integer m_numerator;
        /** The denominator. */
        integer m_denominator;
    };

    std::ostream& operator<<(std::ostream&, const ratio&);
}

#endif /* !LASKIN_RATIO_HPP_GUARD */
