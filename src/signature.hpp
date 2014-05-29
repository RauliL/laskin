#ifndef LASKIN_SIGNATURE_HPP_GUARD
#define LASKIN_SIGNATURE_HPP_GUARD

#include "defines.hpp"
#include <vector>

namespace laskin
{
    class signature
    {
    public:
        enum type
        {
            type_any,
            type_bool,
            type_number,
            type_integer,
            type_real,
            type_string,
            type_vector
        };

        /**
         * Constructs new function signature with given parameter and return
         * types.
         */
        explicit signature(
                const std::vector<type>& parameter_types = std::vector<type>(),
                const std::vector<type>& return_types = std::vector<type>()
        );

        /**
         * Copy constructor.
         */
        signature(const signature& that);

        inline const std::vector<type>& parameter_types() const
        {
            return m_parameter_types;
        }

        inline const std::vector<type>& return_types() const
        {
            return m_return_types;
        }

        signature& assign(const signature& that);

        /**
         * Assignment operator.
         */
        inline signature& operator=(const signature& that)
        {
            return assign(that);
        }

    private:
        std::vector<type> m_parameter_types;
        std::vector<type> m_return_types;
    };
}

#endif /* !LASKIN_SIGNATURE_HPP_GUARD */
