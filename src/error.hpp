#ifndef LASKIN_ERROR_HPP_GUARD
#define LASKIN_ERROR_HPP_GUARD

#include <exception>
#include <string>

namespace laskin
{
    class error : public std::exception
    {
    public:
        enum type
        {
            /** Script wants to exit. */
            type_exit,
            /** Syntax error. */
            type_syntax,
            /** A reference error. */
            type_reference,
            /** A type error. */
            type_type,
            /** A range error. */
            type_range,
            /** Unknown error. */
            type_unknown
        };

        explicit error(enum type type = type_unknown,
                       const std::string& message = std::string());

        inline enum type type() const
        {
            return m_type;
        }

        inline bool is(enum type type) const
        {
            return m_type == type;
        }

        inline const std::string& message() const
        {
            return m_message;
        }

        inline const char* what() const
            throw()
        {
            return m_message.c_str();
        }

        error(const error& that);

        error& assign(const error& that);

        /**
         * Assignment operator.
         */
        inline error& operator=(const error& that)
        {
            return assign(that);
        }

    private:
        /** Type of the error. */
        enum type m_type;
        /** Optional error message given. */
        std::string m_message;
    };
}

#endif /* !LASKIN_ERROR_HPP_GUARD */
