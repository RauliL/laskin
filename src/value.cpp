#include "function.hpp"
#include "numbers.hpp"
#include "value.hpp"
#include <cmath>

namespace laskin
{
    value::value()
        : m_type(type_int)
    {
        m_data.i = 0;
    }

    value::value(const value& that)
        : m_type(that.m_type)
    {
        switch (m_type)
        {
            case type_bool:
            case type_int:
                m_data.i = that.m_data.i;
                break;

            case type_real:
                m_data.r = that.m_data.r;
                break;

            case type_ratio:
                m_data.rat = new ratio(*that.m_data.rat);
                break;

            case type_string:
                m_data.s.container = that.m_data.s.container;
                m_data.s.counter = that.m_data.s.counter;
                ++(*m_data.s.counter);
                break;

            case type_list:
                m_data.l.container = that.m_data.l.container;
                m_data.l.counter = that.m_data.l.counter;
                ++(*m_data.l.counter);
                break;

            case type_function:
                m_data.f.function = that.m_data.f.function;
                m_data.f.counter = that.m_data.f.counter;
                ++(*m_data.f.counter);
        }
    }

    value::value(bool b)
        : m_type(type_bool)
    {
        m_data.i = b ? 1 : 0;
    }

    value::value(integer i)
        : m_type(type_int)
    {
        m_data.i = i;
    }

    value::value(real r)
        : m_type(type_real)
    {
        m_data.r = r;
    }

    value::value(const class ratio& ratio)
        : m_type(type_ratio)
    {
        m_data.rat = new class ratio(ratio);
    }

    value::value(const std::string& s)
        : m_type(type_string)
    {
        m_data.s.container = new std::string(s);
        m_data.s.counter = new unsigned(1);
    }

    value::value(const std::vector<value>& l)
        : m_type(type_list)
    {
        m_data.l.container = new std::vector<value>(l);
        m_data.l.counter = new unsigned(1);
    }

    value::value(const function& f)
        : m_type(type_function)
    {
        m_data.f.function = new function(f);
        m_data.f.counter = new unsigned(1);
    }

    value::~value()
    {
        if (m_type == type_string)
        {
            if (!--(*m_data.s.counter))
            {
                delete m_data.s.counter;
                delete m_data.s.container;
            }
        }
        else if (m_type == type_list)
        {
            if (!--(*m_data.l.counter))
            {
                delete m_data.l.counter;
                delete m_data.l.container;
            }
        }
        else if (m_type == type_function)
        {
            if (!--(*m_data.f.counter))
            {
                delete m_data.f.counter;
                delete m_data.f.function;
            }
        }
        else if (m_type == type_ratio)
        {
            delete m_data.rat;
        }
    }

    integer value::as_int() const
        throw(std::out_of_range)
    {
        if (m_type == type_int)
        {
            return m_data.i;
        }
        else if (m_type == type_real)
        {
            real r = m_data.r;

            if (r > 0.0)
            {
                r = std::floor(r);
            }
            if (r < 0.0)
            {
                r = std::ceil(r);
            }
            if (!LASKIN_IS_IN_INTEGER_RANGE(r))
            {
                throw std::out_of_range("real number out of integer bounds");
            }

            return static_cast<integer>(r);
        }
        else if (m_type == type_ratio)
        {
            const integer numerator = m_data.rat->numerator();
            const integer denominator = m_data.rat->denominator();

            if (numerator < 0)
            {
                return -(-numerator / denominator);
            } else {
                return numerator / denominator;
            }
        }

        return 0;
    }

    real value::as_real() const
    {
        if (m_type == type_real)
        {
            return m_data.r;
        }
        else if (m_type == type_int)
        {
            return static_cast<real>(m_data.i);
        }
        else if (m_type == type_ratio)
        {
            return static_cast<real>(m_data.rat->numerator())
                / static_cast<real>(m_data.rat->denominator());
        }

        return 0.0;
    }

    value& value::assign(const value& that)
    {
        if (m_type == type_string)
        {
            if (!--(*m_data.s.counter))
            {
                delete m_data.s.counter;
                delete m_data.s.container;
            }
        }
        else if (m_type == type_list)
        {
            if (!--(*m_data.l.counter))
            {
                delete m_data.l.counter;
                delete m_data.l.container;
            }
        }
        else if (m_type == type_function)
        {
            if (!--(*m_data.f.counter))
            {
                delete m_data.f.counter;
                delete m_data.f.function;
            }
        }
        else if (m_type == type_ratio)
        {
            delete m_data.rat;
        }
        switch (m_type = that.m_type)
        {
            case type_bool:
            case type_int:
                m_data.i = that.m_data.i;
                break;

            case type_real:
                m_data.r = that.m_data.r;
                break;

            case type_ratio:
                m_data.rat = new ratio(*that.m_data.rat);
                break;

            case type_string:
                m_data.s.container = that.m_data.s.container;
                m_data.s.counter = that.m_data.s.counter;
                ++(*m_data.s.counter);
                break;

            case type_list:
                m_data.l.container = that.m_data.l.container;
                m_data.l.counter = that.m_data.l.counter;
                ++(*m_data.l.counter);
                break;

            case type_function:
                m_data.f.function = that.m_data.f.function;
                m_data.f.counter = that.m_data.f.counter;
                ++(*m_data.f.counter);
        }

        return *this;
    }

    bool value::equals(const value& that) const
    {
        switch (m_type)
        {
            case type_bool:
                return that.m_type == type_bool && m_data.i == that.m_data.i;

            case type_int:
                if (that.m_type == type_int)
                {
                    return m_data.i == that.m_data.i;
                }
                else if (that.m_type == type_real)
                {
                    return static_cast<real>(m_data.i) == that.m_data.r;
                }
                break;

            case type_real:
                if (that.m_type == type_real)
                {
                    return m_data.r == that.m_data.r;
                }
                else if (that.m_type == type_int)
                {
                    return m_data.r == static_cast<real>(that.m_data.r);
                }
                break;

            case type_ratio:
                if (that.m_type == type_ratio)
                {
                    return m_data.rat->equals(*that.m_data.rat);
                }
                else if (that.m_type == type_int)
                {
                    return m_data.rat->numerator() == that.m_data.i
                        && m_data.rat->denominator() == 1;
                }
                else if (that.m_type == type_real)
                {
                    return static_cast<real>(m_data.rat->numerator()) == that.m_data.r
                        && m_data.rat->denominator() == 1;
                }
                break;

            case type_string:
                if (that.m_type == type_string)
                {
                    return !m_data.s.container->compare(*that.m_data.s.container);
                }
                break;

            case type_list:
                if (that.m_type == type_list)
                {
                    const std::vector<value>& a = *m_data.l.container;
                    const std::vector<value>& b = *m_data.l.container;

                    if (a.size() != b.size())
                    {
                        return false;
                    }
                    for (std::vector<value>::size_type i = 0; i < a.size(); ++i)
                    {
                        if (a[i] != b[i])
                        {
                            return false;
                        }
                    }

                    return true;
                }
                break;

            case type_function:
                if (that.m_type == type_function)
                {
                    return m_data.f.function->signature()
                        == that.m_data.f.function->signature();
                }
        }

        return false;
    }

    int value::compare(const value& that) const
    {
        switch (m_type)
        {
            case value::type_int:
                if (that.m_type == type_int)
                {
                    if (m_data.i > that.m_data.i)
                    {
                        return 1;
                    }
                    else if (m_data.i < that.m_data.i)
                    {
                        return -1;
                    }
                }
                else if (that.m_type == type_real)
                {
                    const real a = static_cast<real>(m_data.i);
                    const real b = that.m_data.r;

                    if (a > b)
                    {
                        return 1;
                    }
                    else if (a < b)
                    {
                        return -1;
                    }
                }
                else if (that.m_type == type_ratio)
                {
                    const integer a = m_data.i;
                    const integer b = that.m_data.rat->numerator() / that.m_data.rat->denominator();

                    if (a > b)
                    {
                        return 1;
                    }
                    else if (a < b)
                    {
                        return -1;
                    }
                }
                break;

            case value::type_real:
                if (that.m_type == type_real)
                {
                    if (m_data.r > that.m_data.r)
                    {
                        return 1;
                    }
                    else if (m_data.r < that.m_data.r)
                    {
                        return -1;
                    }
                }
                else if (that.m_type == type_int)
                {
                    const real a = m_data.r;
                    const real b = static_cast<real>(that.m_data.r);

                    if (a > b)
                    {
                        return 1;
                    }
                    else if (a < b)
                    {
                        return -1;
                    }
                }
                else if (that.m_type == type_ratio)
                {
                    const real a = m_data.r;
                    const real b = static_cast<real>(that.m_data.rat->numerator()) /
                                   static_cast<real>(that.m_data.rat->denominator());

                    if (a > b)
                    {
                        return 1;
                    }
                    else if (a < b)
                    {
                        return -1;
                    }
                }
                break;

            case value::type_ratio:
                if (that.m_type == type_ratio)
                {
                    return m_data.rat->compare(*that.m_data.rat);
                }
                else if (that.m_type == type_int)
                {
                    const integer a = m_data.rat->numerator() / m_data.rat->denominator();
                    const integer b = that.m_data.i;

                    if (a > b)
                    {
                        return 1;
                    }
                    else if (a < b)
                    {
                        return -1;
                    }
                }
                else if (that.m_type == type_real)
                {
                    const real a = static_cast<real>(m_data.rat->numerator()) /
                                   static_cast<real>(m_data.rat->denominator());
                    const real b = that.m_data.r;

                    if (a > b)
                    {
                        return 1;
                    }
                    else if (a < b)
                    {
                        return -1;
                    }
                }
                break;

            case value::type_string:
                if (that.m_type == type_string)
                {
                    return m_data.s.container->compare(*that.m_data.s.container);
                }
                break;

            case value::type_bool:
            case value::type_list:
            case value::type_function:
                break;
        }

        return 0;
    }

    std::ostream& operator<<(std::ostream& os, const class value& value)
    {
        switch (value.type())
        {
            case value::type_bool:
                os << (value.as_bool() ? "true" : "false");
                break;

            case value::type_int:
                os << int_to_string(value.as_int());
                break;

            case value::type_real:
                os << real_to_string(value.as_real());
                break;

            case value::type_ratio:
                os << value.as_ratio();
                break;

            case value::type_string:
                os << value.as_string();
                break;

            case value::type_list:
            {
                const std::vector<class value>& vec = value.as_list();

                for (std::size_t i = 0; i < vec.size(); ++i)
                {
                    if (i > 0)
                    {
                        os << ", ";
                    }
                    os << vec[i];
                }
                break;
            }

            case value::type_function:
                os << "function" << value.as_function().signature();
                break;
        }
        return os;
    }

    std::ostream& operator<<(std::ostream& os, enum value::type type)
    {
        switch (type)
        {
            case value::type_bool:
                os << "bool";
                break;

            case value::type_int:
                os << "int";
                break;

            case value::type_real:
                os << "real";
                break;

            case value::type_ratio:
                os << "ratio";
                break;

            case value::type_string:
                os << "string";
                break;

            case value::type_list:
                os << "list";
                break;

            case value::type_function:
                os << "function";
        }

        return os;
    }
}
