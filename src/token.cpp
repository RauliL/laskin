#include "token.hpp"

namespace laskin
{
    static inline bool isword(char);
    static inline token str_to_word(const std::string&);

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
                    while (current != end)
                    {
                        if (*current++ == '\n')
                        {
                            break;
                        }
                    }
                    break;

                // Skip whitespace.
                case ' ':
                case '\t':
                case '\r':
                case '\n':
                    break;

                // Various separators.
                case '(': case ')':
                case '[': case ']':
                case '{': case '}':
                case ':':
                    tokens.push_back(token(
                                c == '(' ? type_lparen :
                                c == ')' ? type_rparen :
                                c == '[' ? type_lbrack :
                                c == ']' ? type_rbrack :
                                c == '{' ? type_lbrace :
                                c == '}' ? type_rbrace :
                                type_colon
                    ));
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
                    tokens.push_back(token(type_int, buffer));
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
                            ++current;
SCAN_EXPONENT:
                            buffer.append(1, 'e');
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
                    }
                    else if (current != end && (*current == 'e' || *current == 'E'))
                    {
                        ++current;
                        goto SCAN_REAL;
                    } else {
                        tokens.push_back(token(type_int, buffer));
                    }
                    break;

                // Parse string literals.
                case '"':
                case '\'':
                {
                    const char separator = c;

                    buffer.clear();
                    while (current != end && *current != separator)
                    {
                        if (*current == '\\')
                        {
                            if (++current == end)
                            {
                                throw syntax_error("end of input after escape sequence");
                            }
                            switch (c = *current++)
                            {
                                case 'a':
                                    buffer.append(1, 007);
                                    break;

                                case 'e':
                                    buffer.append(1, 033);
                                    break;

                                case 'n':
                                    buffer.append(1, '\n');
                                    break;

                                case 'r':
                                    buffer.append(1, '\r');
                                    break;

                                case 't':
                                    buffer.append(1, '\t');
                                    break;

                                case '"':
                                case '\'':
                                case '\\':
                                    buffer.append(1, c);
                                    break;

                                default:
                                    throw syntax_error("illegal escape sequence");
                            }
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
                }

                case '-':
                case '+':
                    buffer.assign(1, c);
                    if (current != end)
                    {
                        if (*current == '0')
                        {
                            buffer.append(1, *current++);
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
                        tokens.push_back(str_to_word(buffer));
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

    std::ostream& operator<<(std::ostream& os, const class token& token)
    {
        switch (token.type())
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

            case token::type_int:
            case token::type_real:
                os << token.data();
                break;

            case token::type_string:
                os << "string literal";
                break;

            case token::type_word:
                os << "`" << token.data() << "'";
                break;

            case token::type_keyword_if:
                os << "`if'";
                break;

            case token::type_keyword_else:
                os << "`else'";
                break;

            case token::type_keyword_while:
                os << "`while'";
                break;

            case token::type_keyword_to:
                os << "`to'";
        }

        return os;
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

            case token::type_int:
            case token::type_real:
                os << "number literal";
                break;

            case token::type_string:
                os << "string literal";
                break;

            case token::type_word:
                os << "word";
                break;

            case token::type_keyword_if:
                os << "`if'";
                break;

            case token::type_keyword_else:
                os << "`else'";
                break;

            case token::type_keyword_while:
                os << "`while'";
                break;

            case token::type_keyword_to:
                os << "`to'";
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

    static inline token str_to_word(const std::string& s)
    {
        switch (s[0])
        {
            case 'e':
                if (!s.compare("else"))
                {
                    return token(token::type_keyword_else);
                }
                break;

            case 'i':
                if (!s.compare("if"))
                {
                    return token(token::type_keyword_if);
                }
                break;

            case 't':
                if (!s.compare("to"))
                {
                    return token(token::type_keyword_to);
                }
                break;

            case 'w':
                if (!s.compare("while"))
                {
                    return token(token::type_keyword_while);
                }
                break;
        }

        return token(token::type_word, s);
    }
}
