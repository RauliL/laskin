#include "signature.hpp"
#include "value.hpp"

namespace laskin
{
    signature::signature(const std::vector<entry>& parameter_types,
                         const std::vector<entry>& return_types)
        : m_parameter_types(parameter_types)
        , m_return_types(return_types) {}

    signature::signature(const std::string& source)
    {
        bool in_parameters = true;

        for (auto c : source)
        {
            enum entry entry;

            switch (c)
            {
                case 'b':
                    entry = type_bool;
                    break;

                case 'n':
                    entry = type_num;
                    break;

                case 'i':
                    entry = type_int;
                    break;

                case 'r':
                    entry = type_real;
                    break;

                case 'R':
                    entry = type_ratio;
                    break;

                case 's':
                    entry = type_string;
                    break;

                case 'l':
                    entry = type_list;
                    break;

                case 'f':
                    entry = type_function;
                    break;

                case ':':
                    in_parameters = false;
                    continue;

                default:
                    entry = type_any;
            }
            if (in_parameters)
            {
                m_parameter_types.push_back(entry);
            } else {
                m_return_types.push_back(entry);
            }
        }
    }

    signature::signature(const signature& that)
        : m_parameter_types(that.m_parameter_types)
        , m_return_types(that.m_return_types) {}

    bool signature::test(const class stack<value>& stack) const
    {
        if (stack.size() < m_parameter_types.size())
        {
            return false;
        }

        for (std::size_t i = m_parameter_types.size(), j = stack.size(); i > 0; --i, --j)
        {
            const class value value = stack[j - 1];

            switch (m_parameter_types[i - 1])
            {
                case type_any:
                    break;

                case type_bool:
                    if (!value.is(value::type_bool))
                    {
                        return false;
                    }
                    break;

                case type_num:
                    if (!value.is(value::type_int)
                        && !value.is(value::type_real)
                        && !value.is(value::type_ratio))
                    {
                        return false;
                    }
                    break;

                case type_int:
                    if (!value.is(value::type_int))
                    {
                        return false;
                    }
                    break;

                case type_real:
                    if (!value.is(value::type_real))
                    {
                        return false;
                    }
                    break;

                case type_ratio:
                    if (!value.is(value::type_ratio))
                    {
                        return false;
                    }
                    break;

                case type_string:
                    if (!value.is(value::type_string))
                    {
                        return false;
                    }
                    break;

                case type_list:
                    if (!value.is(value::type_list))
                    {
                        return false;
                    }
                    break;

                case type_function:
                    if (!value.is(value::type_function))
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
        for (std::vector<entry>::size_type i = 0; i < m_parameter_types.size(); ++i)
        {
            const entry a = m_parameter_types[i];
            const entry b = that.m_parameter_types[i];

            if (a != type_any && b != type_any && a != b)
            {
                return false;
            }
        }

        return true;
    }

    std::ostream& operator<<(std::ostream& os, const class signature& signature)
    {
        const std::vector<signature::entry>& parameter_types = signature.parameter_types();
        const std::vector<signature::entry>& return_types = signature.return_types();
        bool first = true;

        os << '(';
        for (const signature::entry& entry : parameter_types)
        {
            if (first)
            {
                first = false;
            } else {
                os << ' ';
            }
            os << entry;
        }
        if (!return_types.empty())
        {
            if (!first)
            {
                os << ' ';
            }
            os << "--";
        }
        for (const signature::entry& entry : return_types)
        {
            os << ' ' << entry;
        }
        os << ')';

        return os;
    }

    std::ostream& operator<<(std::ostream& os, signature::entry entry)
    {
        switch (entry)
        {
            case signature::type_any:
                os << "any";
                break;

            case signature::type_bool:
                os << "bool";
                break;

            case signature::type_num:
                os << "num";
                break;

            case signature::type_int:
                os << "int";
                break;

            case signature::type_real:
                os << "real";
                break;

            case signature::type_ratio:
                os << "ratio";
                break;

            case signature::type_string:
                os << "string";
                break;

            case signature::type_list:
                os << "list";
                break;

            case signature::entry::type_function:
                os << "function";
        }

        return os;
    }
}
