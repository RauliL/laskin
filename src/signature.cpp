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

        for (auto& c : source)
        {
            enum entry::type type;

            switch (c)
            {
                case 'b':
                    type = entry::type_bool;
                    break;

                case 'n':
                    type = entry::type_num;
                    break;

                case 'i':
                    type = entry::type_int;
                    break;

                case 'r':
                    type = entry::type_real;
                    break;

                case 'R':
                    type = entry::type_ratio;
                    break;

                case 's':
                    type = entry::type_string;
                    break;

                case 'l':
                    type = entry::type_list;
                    break;

                case 'f':
                    type = entry::type_function;
                    break;

                case ':':
                    in_parameters = false;
                    continue;

                default:
                    type = entry::type_any;
            }
            if (in_parameters)
            {
                m_parameter_types.push_back(entry(type));
            } else {
                m_return_types.push_back(entry(type));
            }
        }
    }

    signature::signature(const signature& that)
        : m_parameter_types(that.m_parameter_types)
        , m_return_types(that.m_return_types) {}

    signature& signature::assign(const signature& that)
    {
        m_parameter_types = that.m_parameter_types;
        m_return_types = that.m_return_types;

        return *this;
    }

    bool signature::test(const stack<value>& operands) const
    {
        if (operands.size() < m_parameter_types.size())
        {
            return false;
        }
        for (std::size_t i = m_parameter_types.size(), j = operands.size();
             i > 0;
             --i, --j)
        {
            const class value& value = operands[j - 1];
            const class entry& entry = m_parameter_types[i - 1];

            switch (entry.type())
            {
                case entry::type_any:
                    break;

                case entry::type_bool:
                    if (!value.is(value::type_bool))
                    {
                        return false;
                    }
                    break;

                case entry::type_num:
                    if (!value.is(value::type_int)
                        && !value.is(value::type_real)
                        && !value.is(value::type_ratio))
                    {
                        return false;
                    }
                    break;

                case entry::type_int:
                    if (!value.is(value::type_int))
                    {
                        return false;
                    }
                    break;

                case entry::type_real:
                    if (!value.is(value::type_real))
                    {
                        return false;
                    }
                    break;

                case entry::type_ratio:
                    if (!value.is(value::type_ratio))
                    {
                        return false;
                    }
                    break;

                case entry::type_string:
                    if (!value.is(value::type_string))
                    {
                        return false;
                    }
                    break;

                case entry::type_list:
                    if (!value.is(value::type_list))
                    {
                        return false;
                    }
                    break;

                case entry::type_function:
                    if (!value.is(value::type_function))
                    {
                        return false;
                    }
            }
        }

        return true;
    }

    bool signature::equals(const signature& that) const
    {
        if (m_parameter_types.size() != that.m_parameter_types.size())
        {
            return false;
        }
        for (std::vector<entry>::size_type i = 0; i < m_parameter_types.size(); ++i)
        {
            const entry& a = m_parameter_types[i];
            const entry& b = that.m_parameter_types[i];

            if (a != b)
            {
                return false;
            }
        }

        return true;
    }

    signature::entry::entry(enum type type)
        : m_type(type) {}

    signature::entry::entry(const entry& that)
        : m_type(that.m_type) {}

    bool signature::entry::equals(const entry& that) const
    {
        return m_type == type_any
            || that.m_type == type_any
            || m_type == that.m_type;
    }

    signature::entry& signature::entry::assign(const entry& that)
    {
        m_type = that.m_type;

        return *this;
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

    std::ostream& operator<<(std::ostream& os, const signature::entry& entry)
    {
        return os << entry.type();
    }

    std::ostream& operator<<(std::ostream& os, enum signature::entry::type type)
    {
        switch (type)
        {
            case signature::entry::type_any:
                os << "any";
                break;

            case signature::entry::type_bool:
                os << "bool";
                break;

            case signature::entry::type_num:
                os << "num";
                break;

            case signature::entry::type_int:
                os << "int";
                break;

            case signature::entry::type_real:
                os << "real";
                break;

            case signature::entry::type_ratio:
                os << "ratio";
                break;

            case signature::entry::type_string:
                os << "string";
                break;

            case signature::entry::type_list:
                os << "list";
                break;

            case signature::entry::type_function:
                os << "function";
        }

        return os;
    }
}
