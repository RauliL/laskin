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
                m_data.s = new std::string(*that.m_data.s);
                break;

            case type_vector:
                m_data.v = new std::vector<value>(*that.m_data.v);
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
        m_data.s = new std::string(s);
    }

    value::value(const std::vector<value>& v)
        : m_type(type_vector)
    {
        m_data.v = new std::vector<value>(v);
    }

    value::~value()
    {
        if (m_type == type_string)
        {
            delete m_data.s;
        }
        else if (m_type == type_vector)
        {
            delete m_data.v;
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
            delete m_data.s;
        }
        else if (m_type == type_vector)
        {
            delete m_data.v;
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
                m_data.s = new std::string(*that.m_data.s);
                break;

            case type_vector:
                m_data.v = new std::vector<value>(*that.m_data.v);
        }

        return *this;
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

            case value::type_vector:
            {
                const std::vector<class value>& vec = value.as_vector();

                for (std::size_t i = 0; i < vec.size(); ++i)
                {
                    if (i > 0)
                    {
                        os << ", ";
                    }
                    os << vec[i];
                }
            }
        }
        return os;
    }
}
