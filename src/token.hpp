#ifndef LASKIN_TOKEN_HPP_GUARD
#define LASKIN_TOKEN_HPP_GUARD

#include "error.hpp"
#include <iostream>
#include <vector>

namespace laskin
{
    class token
    {
    public:
        enum type
        {
            type_lparen,
            type_rparen,
            type_lbrack,
            type_rbrack,
            type_lbrace,
            type_rbrace,
            type_colon,
            type_int,
            type_real,
            type_string,
            type_word,
            type_keyword_if,
            type_keyword_else,
            type_keyword_while,
            type_keyword_to
        };

        /**
         * Constructs new token.
         *
         * \param type Type of the token
         * \param data Text data associated with the token
         */
        explicit token(enum type type = type_word,
                       const std::string& data = std::string());

        /**
         * Copy constructor.
         */
        token(const token& that);

        static std::vector<token> scan(std::istream& is)
            throw(syntax_error);

        /**
         * Returns type of the token.
         */
        inline enum type type() const
        {
            return m_type;
        }

        /**
         * Tests whether token is given type.
         */
        inline bool is(enum type type) const
        {
            return m_type == type;
        }

        /**
         * Returns text data associated with the token.
         */
        inline const std::string& data() const
        {
            return m_data;
        }

        /**
         * Copies contents from another token into this one.
         */
        token& assign(const token& that);

        /**
         * Assignment operator.
         */
        inline token& operator=(const token& that)
        {
            return assign(that);
        }

    private:
        /** Type of the token. */
        enum type m_type;
        /** Text data associated with token. */
        std::string m_data;
    };

    std::ostream& operator<<(std::ostream&, const token&);
    std::ostream& operator<<(std::ostream&, enum token::type);
}

#endif /* !LASKIN_TOKEN_HPP_GUARD */
