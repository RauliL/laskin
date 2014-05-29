#ifndef LASKIN_ERROR_HPP_GUARD
#define LASKIN_ERROR_HPP_GUARD

#include <exception>
#include <string>

namespace laskin
{
    class error : public std::exception
    {
    public:
        explicit error(const std::string& message)
            : m_message(message) {}

        const char* what() const
        {
            return m_message.c_str();
        }

    private:
        std::string m_message;
    };

    class syntax_error : public error
    {
    public:
        explicit syntax_error(const std::string& message)
            : error(message) {}
    };

    class script_error : public error
    {
    public:
        explicit script_error(const std::string& message)
            : error(message) {}
    };
}

#endif /* !LASKIN_ERROR_HPP_GUARD */
