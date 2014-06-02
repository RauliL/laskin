#ifndef LASKIN_SIGNATURE_HPP_GUARD
#define LASKIN_SIGNATURE_HPP_GUARD

#include "defines.hpp"
#include <string>
#include <vector>

namespace laskin
{
    class signature
    {
    public:
        enum entry
        {
            type_any,
            type_bool,
            type_num,
            type_int,
            type_real,
            type_ratio,
            type_string,
            type_list,
            type_function
        };

        /**
         * Constructs new function signature with given parameter and return
         * types.
         */
        explicit signature(
                const std::vector<entry>& parameter_types = std::vector<entry>(),
                const std::vector<entry>& return_types = std::vector<entry>()
        );

        explicit signature(const std::string& source);

        /**
         * Copy constructor.
         */
        signature(const signature& that);

        inline const std::vector<entry>& parameter_types() const
        {
            return m_parameter_types;
        }

        inline const std::vector<entry>& return_types() const
        {
            return m_return_types;
        }

        bool test(const class stack<value>& stack) const;

        signature& assign(const signature& that);

        /**
         * Assignment operator.
         */
        inline signature& operator=(const signature& that)
        {
            return assign(that);
        }

        bool equals(const signature& that) const;

        /**
         * Equality testing operator.
         */
        inline bool operator==(const signature& that) const
        {
            return equals(that);
        }

        /**
         * Non-equality testing operator.
         */
        inline bool operator!=(const signature& that) const
        {
            return !equals(that);
        }

    private:
        std::vector<entry> m_parameter_types;
        std::vector<entry> m_return_types;
    };

    std::ostream& operator<<(std::ostream&, const signature&);
    std::ostream& operator<<(std::ostream&, signature::entry);
}

#endif /* !LASKIN_SIGNATURE_HPP_GUARD */
