#include "interpreter.hpp"
#include "token.hpp"
#include "value.hpp"
#include <sstream>

namespace laskin
{
    typedef std::vector<token>::const_iterator token_iterator;

    static value parse_value(token_iterator&, const token_iterator&);
    static value parse_vector_value(token_iterator&, const token_iterator&);
    static signature parse_signature(token_iterator&, const token_iterator&);
    static void parse_block(token_iterator&, const token_iterator&, std::vector<token>&);

    namespace internal
    {
        void initialize_bool(interpreter*);
        void initialize_io(interpreter*);
        void initialize_list(interpreter*);
        void initialize_numbers(interpreter*);
        void initialize_program(interpreter*);
        void initialize_stack(interpreter*);
        void initialize_string(interpreter*);
    }

    interpreter::interpreter() {}

    interpreter::interpreter(const interpreter& that)
        : m_functions(that.m_functions) {}

    void interpreter::initialize()
    {
        using namespace internal;

        initialize_bool(this);
        initialize_io(this);
        initialize_list(this);
        initialize_numbers(this);
        initialize_program(this);
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

                case token::type_colon:
                {
                    std::string name;
                    class signature signature;
                    std::vector<class token> body;

                    if (current != end && current->is(token::type_word))
                    {
                        name = current++->data();
                    }
                    if (current != end && current->is(token::type_lparen))
                    {
                        signature = parse_signature(++current, end);
                    }
                    if (current != end && current->is(token::type_lbrace))
                    {
                        parse_block(++current, end, body);
                        if (name.empty())
                        {
                            stack.push_back(value(function(signature, body)));
                        } else {
                            register_function(name, signature, body);
                        }
                    } else {
                        throw syntax_error("missing function body");
                    }
                    break;
                }

                case token::type_word:
                {
                    hashmap<std::vector<function> >::entry* e = m_functions.find(token.data());

                    if (e)
                    {
                        bool found = false;

                        for (auto& f : e->value)
                        {
                            if (f.signature().test(stack))
                            {
                                found = true;
                                f.invoke(*this, stack);
                                break;
                            }
                        }
                        if (!found)
                        {
                            throw script_error(
                                    "signature of function `"
                                    + token.data()
                                    + "' does not match with given stack"
                            );
                        }
                    } else {
                        throw script_error(
                                "undefined function `"
                                + token.data()
                                + "'"
                        );
                    }
                }
            }
        }
    }

    void interpreter::register_function(const std::string& name,
                                        const class signature& signature,
                                        void (*callback)(interpreter&, std::deque<value>&))
    {
        hashmap<std::vector<function> >::entry* e = m_functions.find(name);

        if (e)
        {
            for (auto& f : e->value)
            {
                if (signature == f.signature())
                {
                    f = function(signature, callback);
                    return;
                }
            }
            e->value.push_back(function(signature, callback));
        } else {
            std::vector<function> vector;

            vector.push_back(function(signature, callback));
            m_functions.insert(name, vector);
        }
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
        hashmap<std::vector<function> >::entry* e = m_functions.find(name);

        if (e)
        {
            for (auto& f : e->value)
            {
                if (signature == f.signature())
                {
                    f = function(signature, callback);
                    return;
                }
            }
            e->value.push_back(function(signature, callback));
        } else {
            std::vector<function> vector;

            vector.push_back(function(signature, callback));
            m_functions.insert(name, vector);
        }
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

    static signature parse_signature(token_iterator& current,
                                     const token_iterator& end)
    {
        std::vector<signature::entry> parameter_types;
        std::vector<signature::entry> return_types;
        bool in_parameters = true;

        while (current != end && !current->is(token::type_rparen))
        {
            if (current->is(token::type_colon))
            {
                ++current;
                if (in_parameters)
                {
                    in_parameters = false;
                } else {
                    throw syntax_error("multiple `:' found in function signature");
                }
            }
            else if (current->is(token::type_word))
            {
                const std::string& id = current++->data();
                signature::entry entry;

                if (!id.compare("any"))
                {
                    entry = signature::type_any;
                }
                else if (!id.compare("bool"))
                {
                    entry = signature::type_bool;
                }
                else if (!id.compare("number"))
                {
                    entry = signature::type_number;
                }
                else if (!id.compare("integer"))
                {
                    entry = signature::type_integer;
                }
                else if (!id.compare("real"))
                {
                    entry = signature::type_real;
                }
                else if (!id.compare("string"))
                {
                    entry = signature::type_string;
                }
                else if (!id.compare("list"))
                {
                    entry = signature::type_list;
                }
                else if (!id.compare("function"))
                {
                    entry = signature::type_function;
                } else {
                    throw syntax_error("unknown type: `" + id + "'");
                }
                if (in_parameters)
                {
                    parameter_types.push_back(entry);
                } else {
                    return_types.push_back(entry);
                }
            } else {
                std::stringstream ss;

                ss << "unexpected "
                   << current->type()
                   << "; missing type signature";

                throw syntax_error(ss.str());
            }
        }
        if (current == end)
        {
            throw syntax_error("unterminated function signature: missing `)'");
        }
        ++current;

        return signature(parameter_types, return_types);
    }

    static void parse_block(token_iterator& current,
                            const token_iterator& end,
                            std::vector<token>& tokens)
    {
        unsigned int counter = 1;

        while (current != end && counter > 0)
        {
            const class token& token = *current++;

            switch (token.type())
            {
                case token::type_lbrace:
                    ++counter;
                    break;

                case token::type_rbrace:
                    --counter;
                    break;

                default:
                    tokens.push_back(token);
            }
        }
        if (counter > 0)
        {
            throw syntax_error("unterminated block: missing `}'");
        }
    }
}
