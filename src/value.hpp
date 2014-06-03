#ifndef LASKIN_VALUE_HPP_GUARD
#define LASKIN_VALUE_HPP_GUARD

#include "ratio.hpp"
#include <string>
#include <vector>

namespace laskin
{
    class value
    {
    public:
        enum type
        {
            type_bool,
            type_int,
            type_real,
            type_ratio,
            type_string,
            type_list,
            type_function
        };

        /**
         * Constructs value which is integer value of <code>0</code>.
         */
        value();

        /**
         * Copy constructor.
         *
         * \param that Existing value to construct copy of
         */
        value(const value& that);

        /**
         * Constructs boolean value.
         */
        value(bool b);

        /**
         * Constructs integer value.
         */
        value(integer i);

        /**
         * Constructs real value.
         */
        value(real r);

        /**
         * Constructs ratio value.
         */
        value(const class ratio& ratio);

        /**
         * Constructs string value.
         */
        value(const std::string& s);

        /**
         * Constructs list value.
         */
        value(const std::vector<value>& l);

        /**
         * Constructs function value.
         */
        value(const function& f);

        /**
         * Destructor. All data associated with the value will be destroyed.
         */
        virtual ~value();

        /**
         * Returns type of the value.
         */
        inline enum type type() const
        {
            return m_type;
        }

        /**
         * Tests whether value is instance of given type.
         */
        inline bool is(enum type type) const
        {
            return m_type == type;
        }

        inline bool as_bool() const
        {
            return m_data.i != 0;
        }

        integer as_int() const
            throw(std::out_of_range);

        real as_real() const;

        inline const ratio& as_ratio() const
        {
            return *m_data.rat;
        }

        inline const std::string& as_string() const
        {
            return *m_data.s.container;
        }

        inline const std::vector<value>& as_list() const
        {
            return *m_data.l.container;
        }

        inline const function& as_function() const
        {
            return *m_data.f.function;
        }

        /**
         * Assigns data from another value into this one.
         *
         * \param that Other value to copy data from
         */
        value& assign(const value& that);

        /**
         * Assignment operator.
         */
        inline value& operator=(const value& that)
        {
            return assign(that);
        }

        bool equals(const value& that) const;

        /**
         * Equality testing operator.
         */
        inline bool operator==(const value& that) const
        {
            return equals(that);
        }

        /**
         * Non-equality testing operator.
         */
        inline bool operator!=(const value& that) const
        {
            return !equals(that);
        }

        int compare(const value& that) const;

        inline bool operator<(const value& that) const
        {
            return compare(that) < 0;
        }

        inline bool operator>(const value& that) const
        {
            return compare(that) > 0;
        }

        inline bool operator<=(const value& that) const
        {
            return compare(that) <= 0;
        }

        inline bool operator>=(const value& that) const
        {
            return compare(that) >= 0;
        }

    private:
        /** Type of the value. */
        enum type m_type;
        /** Contains data for the value. */
        union
        {
            /** Integer value. */
            integer i;
            /** Floating point decimal value. */
            real r;
            /** Ratio value. */
            ratio* rat;
            /** String value. */
            struct
            {
                std::string* container;
                unsigned* counter;
            } s;
            /** List value. */
            struct
            {
                std::vector<value>* container;
                unsigned* counter;
            } l;
            /** Function value. */
            struct
            {
                class function* function;
                unsigned* counter;
            } f;
        } m_data;
    };

    std::ostream& operator<<(std::ostream&, const value&);
    std::ostream& operator<<(std::ostream&, enum value::type);
}

#endif /* !LASKIN_VALUE_HPP_GUARD */
