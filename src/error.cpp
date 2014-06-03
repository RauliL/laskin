#include "error.hpp"

namespace laskin
{
    error::error(enum type type, const std::string& message)
        : m_type(type)
        , m_message(message) {}

    error::error(const error& that)
        : m_type(that.m_type)
        , m_message(that.m_message) {}

    error& error::assign(const error& that)
    {
        m_type = that.m_type;
        m_message = that.m_message;

        return *this;
    }
}
