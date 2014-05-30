#include "interpreter.hpp"
#include "token.hpp"
#include "value.hpp"
#include <sstream>

namespace laskin
{
    typedef std::vector<token>::const_iterator token_iterator;

    static value parse_value(token_iterator&, const token_iterator&);
    static value parse_vector_value(token_iterator&, const token_iterator&);

    namespace internal
    {
        void initialize_numbers(interpreter*);
        void initialize_stack(interpreter*);
        void initialize_string(interpreter*);
    }

    interpreter::interpreter() {}

    interpreter::interpreter(const interpreter& that)
        : m_functions(that.m_functions) {}

    void interpreter::initialize()
    {
        using namespace internal;

        initialize_numbers(this);
        initialize_stack(this);
        initialize_string(this);
    }

    void interpreter::execute(const std::vector<token>& tokens,
                              std::deque<value>& stack)
        throw(script_error, syntax_error)
    {
        const token_iterator end = tokens.end();

        for (token_iterator current = tokens.begin(); current < end;)
        {
            const class token& token = *current++;

            switch (token.type())
            {
                case token::type_lparen:
                case token::type_rparen:
                case token::type_rbrack:
                case token::type_lbrace:
                case token::type_rbrace:
                case token::type_colon:
                {
                    std::stringstream ss;

                    ss << "unexpected " << token.type();

                    throw syntax_error(ss.str());
                }

                case token::type_integer:
                    stack.push_back(value(std::stoll(token.data())));
                    break;

                case token::type_real:
                    stack.push_back(value(std::stod(token.data())));
                    break;

                case token::type_string:
                    stack.push_back(value(token.data()));
                    break;

                case token::type_lbrack:
                    stack.push_back(parse_vector_value(current, end));
                    break;

                case token::type_word:
                {
                    const hashmap<function>::entry* e = m_functions.find(token.data());

                    if (e)
                    {
                        while (e)
                        {
                            if (e->value.signature().test(stack))
                            {
                                e->value.invoke(*this, stack);
                                return;
                            }
                            e = e->child;
                        }

                        throw script_error(
                                "signature of function `"
                                + token.data()
                                + "' does not match with given stack"
                        );
                    }

                    throw script_error(
                            "undefined function `"
                            + token.data()
                            + "'"
                    );
                }
            }
        }
    }

    void interpreter::register_function(const std::string& name,
                                        const class signature& signature,
                                        void (*callback)(interpreter&, std::deque<value>&))
    {
        hashmap<function>::entry* e = m_functions.find(name);

        while (e)
        {
            if (signature == e->value.signature())
            {
                e->value = function(signature, callback);
                return;
            }
            e = e->child;
        }
        m_functions.insert(name, function(signature, callback));
    }

    void interpreter::register_function(const std::string& name,
                                        const std::string& sig,
                                        void (*callback)(interpreter&, std::deque<value>&))
    {
        register_function(name, signature(sig), callback);
    }

    void interpreter::register_function(const std::string& name,
                                        const class signature& signature,
                                        const std::vector<token>& callback)
    {
        hashmap<function>::entry* e = m_functions.find(name);

        while (e)
        {
            if (signature == e->value.signature())
            {
                e->value = function(signature, callback);
                return;
            }
            e = e->child;
        }
        m_functions.insert(name, function(signature, callback));
    }

    interpreter& interpreter::assign(const interpreter& that)
    {
        m_functions = that.m_functions;

        return *this;
    }

    static value parse_value(token_iterator& current,
                             const token_iterator& end)
    {
        const class token& token = *current++;

        switch (token.type())
        {
            case token::type_integer:
                return value(std::stoll(token.data()));

            case token::type_real:
                return value(std::stod(token.data()));

            case token::type_string:
                return value(token.data());

            case token::type_lbrack:
                return parse_vector_value(current, end);

            default:
            {
                std::stringstream ss;

                ss << "unexpected " << token.type() << "; missing value";

                throw syntax_error(ss.str());
            }
        }
    }

    static value parse_vector_value(token_iterator& current,
                                    const token_iterator& end)
    {
        std::vector<value> elements;

        while (current != end && !current->is(token::type_rbrack))
        {
            elements.push_back(parse_value(current, end));
        }
        if (current == end)
        {
            throw syntax_error("unterminated vector literal");
        }
        ++current;

        return value(elements);
    }
}
