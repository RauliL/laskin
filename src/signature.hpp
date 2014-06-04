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
        class entry
        {
        public:
            enum type
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

            explicit entry(enum type type = type_any);

            entry(const entry& that);

            /**
             * Returns type of the entry.
             */
            inline enum type type() const
            {
                return m_type;
            }

            /**
             * Tests whether entry is specified type.
             */
            inline bool is(enum type type) const
            {
                return m_type == type;
            }

            bool equals(const entry& that) const;

            inline bool operator==(const entry& that) const
            {
                return equals(that);
            }

            inline bool operator!=(const entry& that) const
            {
                return !equals(that);
            }

            entry& assign(const entry& that);

            /**
             * Assignment operator.
             */
            inline entry& operator=(const entry& that)
            {
                return assign(that);
            }

        private:
            enum type m_type;
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

        bool test(const stack<value>& operands) const;

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
    std::ostream& operator<<(std::ostream&, const signature::entry&);
    std::ostream& operator<<(std::ostream&, enum signature::entry::type);
}

#endif /* !LASKIN_SIGNATURE_HPP_GUARD */
