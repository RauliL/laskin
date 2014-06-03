#include "interpreter.hpp"
#include "script.hpp"

namespace laskin
{
    function::function()
        : m_type(type_native)
    {
        m_callback.n = 0;
    }

    function::function(const class signature& signature,
                       callback n)
        : m_type(type_native)
        , m_signature(signature)
    {
        m_callback.n = n;
    }

    function::function(const class signature& signature,
                       const class script& script)
        : m_type(type_script)
        , m_signature(signature)
    {
        m_callback.s = new class script(script);
    }

    function::function(const function& that)
        : m_type(that.m_type)
        , m_signature(that.m_signature)
    {
        if (m_type == type_native)
        {
            m_callback.n = that.m_callback.n;
        } else {
            m_callback.s = new script(*that.m_callback.s);
        }
    }

    function::~function()
    {
        if (m_type == type_script)
        {
            delete m_callback.s;
        }
    }

    void function::invoke(class interpreter& interpreter,
                          stack<value>& data,
                          hashmap<value>& locals,
                          std::istream& in,
                          std::ostream& out) const
        throw(script_error, syntax_error)
    {
        if (m_type == type_native)
        {
            if (m_callback.n)
            {
                m_callback.n(interpreter, data, locals, in, out);
            }
        } else {
            m_callback.s->execute(interpreter, data, locals, in, out);
        }
    }

    function& function::assign(const function& that)
    {
        if (m_type == type_script)
        {
            delete m_callback.s;
        }
        m_signature = that.m_signature;
        if ((m_type = that.m_type) == type_native)
        {
            m_callback.n = that.m_callback.n;
        } else {
            m_callback.s = new script(*that.m_callback.s);
        }

        return *this;
    }

    function& function::assign(const class signature& signature, callback n)
    {
        if (m_type == type_script)
        {
            delete m_callback.s;
        }
        m_signature = signature;
        m_type = type_native;
        m_callback.n = n;

        return *this;
    }

    function& function::assign(const class signature& signature,
                               const class script& script)
    {
        m_signature = signature;
        if (m_type == type_script)
        {
            m_callback.s->assign(script);
        } else {
            m_callback.s = new class script(script);
            m_type = type_script;
        }

        return *this;
    }
}
