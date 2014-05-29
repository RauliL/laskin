#include "signature.hpp"

namespace laskin
{
    signature::signature(const std::vector<type>& parameter_types,
                         const std::vector<type>& return_types)
        : m_parameter_types(parameter_types)
        , m_return_types(return_types) {}

    signature::signature(const signature& that)
        : m_parameter_types(that.m_parameter_types)
        , m_return_types(that.m_return_types) {}

    signature& signature::assign(const signature& that)
    {
        m_parameter_types = that.m_parameter_types;
        m_return_types = that.m_return_types;

        return *this;
    }
}
