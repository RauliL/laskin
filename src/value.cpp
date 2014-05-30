#include "function.hpp"
#include "value.hpp"
#include <cmath>

namespace laskin
{
    value::value()
        : m_type(type_integer)
    {
        m_data.i = 0;
    }

    value::value(const value& that)
        : m_type(that.m_type)
    {
        switch (m_type)
        {
            case type_bool:
            case type_integer:
                m_data.i = that.m_data.i;
                break;

            case type_real:
                m_data.r = that.m_data.r;
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
        : m_type(type_integer)
    {
        m_data.i = i;
    }

    value::value(real r)
        : m_type(type_real)
    {
        m_data.r = r;
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
    }

    integer value::as_integer() const
        throw(std::out_of_range)
    {
        if (m_type == type_integer)
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

        return 0;
    }

    real value::as_real() const
    {
        if (m_type == type_real)
        {
            return m_data.r;
        }
        else if (m_type == type_integer)
        {
            return static_cast<real>(m_data.i);
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
        switch (m_type = that.m_type)
        {
            case type_bool:
            case type_integer:
                m_data.i = that.m_data.i;
                break;

            case type_real:
                m_data.r = that.m_data.r;
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

            case type_integer:
                if (that.m_type == type_integer)
                {
                    return m_data.i == that.m_data.i;
                }
                else if (that.m_type == type_real)
                {
                    return static_cast<double>(m_data.i) == that.m_data.r;
                }
                break;

            case type_real:
                if (that.m_type == type_real)
                {
                    return m_data.r == that.m_data.r;
                }
                else if (that.m_type == type_integer)
                {
                    return m_data.r == static_cast<double>(that.m_data.r);
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

    std::ostream& operator<<(std::ostream& os, const class value& value)
    {
        switch (value.type())
        {
            case value::type_bool:
                os << (value.as_bool() ? "true" : "false");
                break;

            case value::type_integer:
                os << value.as_integer();
                break;

            case value::type_real:
                os << value.as_real();
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
}
