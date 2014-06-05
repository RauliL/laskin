#include "function.hpp"
#include "value.hpp"

namespace laskin
{
    static signature parse_signature(std::string::const_iterator&,
                                     const std::string::const_iterator&);

    signature::signature(const std::vector<entry>& parameter_types,
                         const std::vector<entry>& return_types)
        : m_parameter_types(parameter_types)
        , m_return_types(return_types) {}

    signature::signature(const signature& that)
        : m_parameter_types(that.m_parameter_types)
        , m_return_types(that.m_return_types) {}

    signature signature::parse(const std::string& source)
    {
        std::string::const_iterator begin = source.begin();
        std::string::const_iterator end = source.end();

        return parse_signature(begin, end);
    }

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
            if (!m_parameter_types[i - 1].test(operands[j - 1]))
            {
                return false;
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
        : m_type(type)
        , m_signature(0) {}

    signature::entry::entry(const class signature& signature)
        : m_type(type_function)
        , m_signature(new class signature(signature)) {}

    signature::entry::entry(const entry& that)
        : m_type(that.m_type)
        , m_signature(that.m_signature ? new signature(*that.m_signature) : 0) {}

    signature::entry::~entry()
    {
        if (m_signature)
        {
            delete m_signature;
        }
    }

    bool signature::entry::test(const class value& value) const
    {
        switch (m_type)
        {
            case type_any:
                return true;

            case type_bool:
                return value.is(value::type_bool);

            case type_num:
                return value.is(value::type_int)
                    || value.is(value::type_real)
                    || value.is(value::type_ratio);

            case type_int:
                return value.is(value::type_int);

            case type_real:
                return value.is(value::type_real);
                
            case type_ratio:
                return value.is(value::type_ratio);

            case type_string:
                return value.is(value::type_string);

            case type_list:
                return value.is(value::type_list);

            case type_function:
                if (value.is(value::type_function))
                {
                    if (m_signature)
                    {
                        return m_signature->equals(value.as_function().signature());
                    } else {
                        return true;
                    }
                }
        }

        return false;
    }

    bool signature::entry::equals(const entry& that) const
    {
        if (m_type == type_any || that.m_type == type_any)
        {
            return true;
        }
        else if (m_type == type_function)
        {
            if (that.m_type == type_function)
            {
                if (m_signature && that.m_signature)
                {
                    return m_signature->equals(*that.m_signature);
                } else {
                    return true;
                }
            } else {
                return false;
            }
        } else {
            return m_type == that.m_type;
        }
    }

    signature::entry& signature::entry::assign(const entry& that)
    {
        m_type = that.m_type;
        if (m_signature)
        {
            delete m_signature;
            m_signature = 0;
        }
        if (that.m_signature)
        {
            m_signature = new signature(*that.m_signature);
        }

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

    static signature parse_signature(std::string::const_iterator& current,
                                     const std::string::const_iterator& end)
    {
        std::vector<signature::entry> parameter_types;
        std::vector<signature::entry> return_types;
        bool in_parameters = true;

        while (current < end)
        {
            const char c = *current++;

            switch (c)
            {
                case ':':
                    in_parameters = false;
                    break;

                case '?':
                    if (in_parameters)
                    {
                        parameter_types.push_back(signature::entry(signature::entry::type_any));
                    } else {
                        return_types.push_back(signature::entry(signature::entry::type_any));
                    }
                    break;

                case 'b':
                    if (in_parameters)
                    {
                        parameter_types.push_back(signature::entry(signature::entry::type_bool));
                    } else {
                        return_types.push_back(signature::entry(signature::entry::type_bool));
                    }
                    break;

                case 'n':
                    if (in_parameters)
                    {
                        parameter_types.push_back(signature::entry(signature::entry::type_num));
                    } else {
                        return_types.push_back(signature::entry(signature::entry::type_num));
                    }
                    break;

                case 'i':
                    if (in_parameters)
                    {
                        parameter_types.push_back(signature::entry(signature::entry::type_int));
                    } else {
                        return_types.push_back(signature::entry(signature::entry::type_int));
                    }
                    break;

                case 'r':
                    if (in_parameters)
                    {
                        parameter_types.push_back(signature::entry(signature::entry::type_real));
                    } else {
                        return_types.push_back(signature::entry(signature::entry::type_real));
                    }
                    break;

                case 'R':
                    if (in_parameters)
                    {
                        parameter_types.push_back(signature::entry(signature::entry::type_ratio));
                    } else {
                        return_types.push_back(signature::entry(signature::entry::type_ratio));
                    }
                    break;

                case 's':
                    if (in_parameters)
                    {
                        parameter_types.push_back(signature::entry(signature::entry::type_string));
                    } else {
                        return_types.push_back(signature::entry(signature::entry::type_string));
                    }
                    break;

                case 'l':
                    if (in_parameters)
                    {
                        parameter_types.push_back(signature::entry(signature::entry::type_list));
                    } else {
                        return_types.push_back(signature::entry(signature::entry::type_list));
                    }
                    break;

                case 'f':
                    if (current < end && *current == '(')
                    {
                        std::string::const_iterator begin = ++current;

                        while (current < end && *current != ')')
                        {
                            ++current;
                        }
                        if (in_parameters)
                        {
                            parameter_types.push_back(signature::entry(signature::parse(std::string(begin, current))));
                        } else {
                            return_types.push_back(signature::entry(signature::parse(std::string(begin, current))));
                        }
                        if (current < end)
                        {
                            ++current;
                        }
                    }
                    else if (in_parameters)
                    {
                        parameter_types.push_back(signature::entry(signature::entry::type_function));
                    } else {
                        return_types.push_back(signature::entry(signature::entry::type_function));
                    }
                    break;

                default:
                    break;
            }
        }

        return signature(parameter_types, return_types);
    }
}
