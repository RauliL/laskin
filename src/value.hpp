#ifndef LASKIN_VALUE_HPP_GUARD
#define LASKIN_VALUE_HPP_GUARD

#include "defines.hpp"
#include <iostream>
#include <stdexcept>
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
            type_integer,
            type_real,
            type_string,
            type_list
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
         * Constructs string value.
         */
        value(const std::string& s);

        /**
         * Constructs list value.
         */
        value(const std::vector<value>& l);

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

        integer as_integer() const
            throw(std::out_of_range);

        real as_real() const;

        inline const std::string& as_string() const
        {
            return *m_data.s.container;
        }

        inline const std::vector<value>& as_list() const
        {
            return *m_data.l.container;
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
        } m_data;
    };

    std::ostream& operator<<(std::ostream&, const value&);
}

#endif /* !LASKIN_VALUE_HPP_GUARD */
