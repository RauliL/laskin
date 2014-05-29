#include "function.hpp"

namespace laskin
{
    function::function()
        : m_type(type_native)
    {
        m_callback.c = NULL;
    }

    function::function(const function& that)
        : m_type(that.m_type)
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
        if ((m_type = that.m_type) == type_native)
        {
            m_callback.n = that.m_callback.n;
        } else {
            m_callback.c = new std::vector<token>(*that.m_callback.c);
        }

        return *this;
    }
}
