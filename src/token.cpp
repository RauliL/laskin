#include "token.hpp"

namespace laskin
{
    static inline bool isword(char);

    token::token(enum type type, const std::string& data)
        : m_type(type)
        , m_data(data) {}

    token::token(const token& that)
        : m_type(that.m_type)
        , m_data(that.m_data) {}

    std::vector<token> token::scan(std::istream& is)
        throw(syntax_error)
    {
        std::vector<token> tokens;
        std::string buffer;

        for (std::istreambuf_iterator<char> current(is), end; current != end;)
        {
            char c = *current++;

            switch (c)
            {
                // Skip comments.
                case '#':

                // Skip whitespace.
                case ' ':
                case '\t':
                case '\r':
                case '\n':
                    break;

                // Parse numbers from zero.
                case '0':
                    buffer.assign(1, '0');
                    if (current != end)
                    {
SCAN_NUMBER_FROM_ZERO:
                        switch (c = *current++)
                        {
                            case 'b': case 'B':
                                buffer.append(1, c);
                                while (current != end && std::isdigit(*current))
                                {
                                    if ((c = *current++) != '0' && c != '1')
                                    {
                                        throw syntax_error(
                                                "invalid binary digit"
                                        );
                                    }
                                    buffer.append(1, c);
                                }
                                break;

                            case 'x': case 'X':
                                buffer.append(1, 'x');
                                while (current != end && std::isxdigit(*current))
                                {
                                    buffer.append(1, *current++);
                                }
                                break;

                            case 'o': case 'O':
                            case '0': case '1':
                            case '2': case '3':
                            case '4': case '5':
                            case '6': case '7':
                                buffer.append(1, c);
                                while (current != end && std::isdigit(*current))
                                {
                                    if ((c = *current++) > '7')
                                    {
                                        throw syntax_error(
                                                "invalid octal digit"
                                        );
                                    }
                                    buffer.append(1, c);
                                }
                                break;

                            case '8': case '9':
                                throw syntax_error("invalid octal digit");

                            case 'e': case 'E':
                                goto SCAN_EXPONENT;

                            case '.':
                                goto SCAN_REAL;
                        }
                    }
                    tokens.push_back(token(type_integer, buffer));
                    break;

                // Parse numbers.
                case '1': case '2': case '3':
                case '4': case '5': case '6':
                case '7': case '8': case '9':
                    buffer.assign(1, c);
SCAN_NUMBER:
                    while (current != end && std::isdigit(*current))
                    {
                        buffer.append(1, *current++);
                    }
                    if (current != end && *current == '.')
                    {
                        ++current;
SCAN_REAL:
                        buffer.append(1, '.');
                        if (current == end || !std::isdigit(*current))
                        {
                            throw syntax_error("missing digits after `.'");
                        }
                        do
                        {
                            buffer.append(1, *current++);
                        }
                        while (current != end && std::isdigit(*current));
                        if (current != end && (*current == 'e' || *current == 'E'))
                        {
SCAN_EXPONENT:
                            buffer.append(1, 'e');
                            ++current;
                            if (current != end && (*current == '+' || *current == '-'))
                            {
                                buffer.append(1, *current++);
                            }
                            if (current == end || !std::isdigit(*current))
                            {
                                throw syntax_error("missing exponent");
                            }
                            do
                            {
                                buffer.append(1, *current++);
                            }
                            while (current != end && std::isdigit(*current));
                        }
                        tokens.push_back(token(type_real, buffer));
                    } else {
                        tokens.push_back(token(type_integer, buffer));
                    }
                    break;

                // Parse double quoted strings.
                case '"':
                    buffer.clear();
                    while (current != end && *current != '"')
                    {
                        if (*current == '\\')
                        {
                            // TODO: process escape sequence
                        } else {
                            buffer.append(1, *current++);
                        }
                    }
                    if (current == end)
                    {
                        throw syntax_error("unterminated string literal");
                    }
                    tokens.push_back(token(type_string, buffer));
                    ++current;
                    break;

                // Parse single quoted strings.
                case '\'':
                    buffer.clear();
                    while (current != end && *current != '\'')
                    {
                        if (*current == '\\')
                        {
                            // TODO: process escape sequence
                        } else {
                            buffer.append(1, *current++);
                        }
                    }
                    if (current == end)
                    {
                        throw syntax_error("unterminated string literal");
                    }
                    tokens.push_back(token(type_string, buffer));
                    ++current;
                    break;

                case '-':
                case '+':
                    buffer.assign(1, c);
                    if (current != end)
                    {
                        if (*current == '0')
                        {
                            goto SCAN_NUMBER_FROM_ZERO;
                        }
                        else if (std::isdigit(*current))
                        {
                            goto SCAN_NUMBER;
                        }
                    }
                    goto SCAN_WORD;

                default:
                    if (isword(c))
                    {
                        buffer.assign(1, c);
SCAN_WORD:
                        while (current != end && isword(*current))
                        {
                            buffer.append(1, *current++);
                        }
                        tokens.push_back(token(type_word, buffer));
                    } else {
                        throw syntax_error("unexpected input");
                    }
            }
        }

        return tokens;
    }

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

    static inline bool isword(char c)
    {
        return (c >= '0' && c <= '9')
            || (c >= 'a' && c <= 'z')
            || (c >= 'A' && c <= 'Z')
            || c == '!'
            || c == '$'
            || c == '%'
            || c == '&'
            || c == '\''
            || c == '*'
            || c == '+'
            || c == ','
            || c == '-'
            || c == '.'
            || c == '/'
            || c == ':'
            || c == ';'
            || c == '<'
            || c == '>'
            || c == '='
            || c == '?'
            || c == '@'
            || c == '^'
            || c == '_'
            || c == '`'
            || c == '|'
            || c == '~';
    }
}
