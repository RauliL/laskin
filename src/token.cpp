#include "token.hpp"

namespace laskin
{
    token::token(enum type type, const std::string& data)
        : m_type(type)
        , m_data(data) {}

    token::token(const token& that)
        : m_type(that.m_type)
        , m_data(that.m_data) {}

    token& token::assign(const token& that)
    {
        m_type = that.m_type;
        m_data = that.m_data;

        return *this;
    }

    std::ostream& operator<<(std::ostream& os, enum token::type type)
    {
        switch (type)
        {
            case token::type_lparen:
                os << "`('";
                break;

            case token::type_rparen:
                os << "`)'";
                break;

            case token::type_lbrack:
                os << "`['";
                break;

            case token::type_rbrack:
                os << "`]'";
                break;

            case token::type_lbrace:
                os << "`{'";
                break;

            case token::type_rbrace:
                os << "`}'";
                break;

            case token::type_colon:
                os << "`:'";
                break;

            case token::type_integer:
            case token::type_real:
                os << "number literal";
                break;

            case token::type_string:
                os << "string literal";
                break;

            case token::type_word:
                os << "word";
        }

        return os;
    }
}
