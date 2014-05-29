#include "signature.hpp"
#include "value.hpp"

namespace laskin
{
    signature::signature(const std::vector<type>& parameter_types,
                         const std::vector<type>& return_types)
        : m_parameter_types(parameter_types)
        , m_return_types(return_types) {}

    signature::signature(const std::string& source)
    {
        bool in_parameters = true;

        for (auto c : source)
        {
            enum type type;

            switch (c)
            {
                case 'b':
                    type = type_bool;
                    break;

                case 'n':
                    type = type_number;
                    break;

                case 'i':
                    type = type_integer;
                    break;

                case 'r':
                    type = type_real;
                    break;

                case 's':
                    type = type_string;
                    break;

                case 'v':
                    type = type_vector;
                    break;

                case ':':
                    in_parameters = false;
                    break;

                default:
                    type = type_any;
            }
            if (in_parameters)
            {
                m_parameter_types.push_back(type);
            } else {
                m_return_types.push_back(type);
            }
        }
    }

    signature::signature(const signature& that)
        : m_parameter_types(that.m_parameter_types)
        , m_return_types(that.m_return_types) {}

    bool signature::test(const std::deque<value>& stack) const
    {
        if (stack.size() < m_parameter_types.size())
        {
            return false;
        }

        auto a = m_parameter_types.begin();
        auto b = stack.rbegin();

        while (a != m_parameter_types.end())
        {
            auto v = *b++;

            switch (*a++)
            {
                case type_any:
                    break;

                case type_bool:
                    if (!v.is(value::type_bool))
                    {
                        return false;
                    }
                    break;

                case type_number:
                    if (!v.is(value::type_integer) && !v.is(value::type_real))
                    {
                        return false;
                    }
                    break;

                case type_integer:
                    if (!v.is(value::type_integer))
                    {
                        return false;
                    }
                    break;

                case type_real:
                    if (!v.is(value::type_real))
                    {
                        return false;
                    }
                    break;

                case type_string:
                    if (!v.is(value::type_string))
                    {
                        return false;
                    }
                    break;

                case type_vector:
                    if (!v.is(value::type_vector))
                    {
                        return false;
                    }
            }
        }

        return true;
    }

    signature& signature::assign(const signature& that)
    {
        m_parameter_types = that.m_parameter_types;
        m_return_types = that.m_return_types;

        return *this;
    }

    bool signature::equals(const signature& that) const
    {
        if (m_parameter_types.size() != that.m_parameter_types.size())
        {
            return false;
        }
        for (std::vector<type>::size_type i = 0; i < m_parameter_types.size(); ++i)
        {
            const type a = m_parameter_types[i];
            const type b = that.m_parameter_types[i];

            if (a != type_any && b != type_any && a != b)
            {
                return false;
            }
        }

        return true;
    }
}
