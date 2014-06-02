#include "interpreter.hpp"

namespace laskin
{
    function::function()
        : m_type(type_native)
    {
        m_callback.c = 0;
    }

    function::function(const class signature& signature,
                       callback n)
        : m_type(type_native)
        , m_signature(signature)
    {
        m_callback.n = n;
    }

    function::function(const class signature& signature,
                       const std::vector<token>& c)
        : m_type(type_custom)
        , m_signature(signature)
    {
        m_callback.c = new std::vector<token>(c);
    }

    function::function(const function& that)
        : m_type(that.m_type)
        , m_signature(that.m_signature)
    {
        if (m_type == type_native)
        {
            m_callback.n = that.m_callback.n;
        } else {
            m_callback.c = new std::vector<token>(*that.m_callback.c);
        }
    }

    function::~function()
    {
        if (m_type == type_custom)
        {
            delete m_callback.c;
        }
    }

    void function::invoke(class interpreter& interpreter,
                          class stack<value>& stack,
                          hashmap<value>& local_variables,
                          std::istream& in,
                          std::ostream& out) const
        throw(script_error, syntax_error)
    {
        if (m_type == type_native)
        {
            if (m_callback.n)
            {
                m_callback.n(interpreter, stack, local_variables, in, out);
            }
        } else {
            interpreter.execute(*m_callback.c, stack, local_variables, in, out);
        }
    }

    function& function::assign(const function& that)
    {
        if (m_type == type_custom)
        {
            delete m_callback.c;
        }
        m_signature = that.m_signature;
        if ((m_type = that.m_type) == type_native)
        {
            m_callback.n = that.m_callback.n;
        } else {
            m_callback.c = new std::vector<token>(*that.m_callback.c);
        }

        return *this;
    }
}
