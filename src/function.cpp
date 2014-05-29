#include "function.hpp"

namespace laskin
{
    function::function()
        : m_type(type_native)
    {
        m_callback.c = NULL;
    }

    function::function(const class signature& signature,
                       void (*callback)(interpreter&, std::deque<value>&))
        : m_type(type_native)
        , m_signature(signature)
    {
        m_callback.n = callback;
    }

    function::function(const class signature& signature,
                       const std::vector<token>& callback)
        : m_type(type_custom)
        , m_signature(signature)
    {
        m_callback.c = new std::vector<token>(callback);
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
        if (m_type == type_native)
        {
            delete m_callback.c;
        }
    }

    function& function::assign(const function& that)
    {
        if (m_type == type_native)
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
