#include "interpreter.hpp"
#include "numbers.hpp"
#include "token.hpp"
#include "value.hpp"
#include <sstream>

namespace laskin
{
    typedef std::vector<token>::const_iterator token_iterator;

    static value parse_value(token_iterator&, const token_iterator&);
    static std::vector<value> parse_list(token_iterator&, const token_iterator&);
    static signature parse_function_signature(token_iterator&, const token_iterator&);
    static std::vector<token> parse_block(token_iterator&, const token_iterator&);
    static void parse_if(token_iterator&,
                         const token_iterator&,
                         interpreter&,
                         stack<value>&,
                         hashmap<value>&,
                         std::istream&,
                         std::ostream&);
    static void parse_while(token_iterator&,
                            const token_iterator&,
                            interpreter&,
                            stack<value>&,
                            hashmap<value>&,
                            std::istream&,
                            std::ostream&);

    namespace internal
    {
        void initialize_bool(interpreter*);
        void initialize_io(interpreter*);
        void initialize_list(interpreter*);
        void initialize_numbers(interpreter*);
        void initialize_program(interpreter*);
        void initialize_stack(interpreter*);
        void initialize_string(interpreter*);
        void initialize_trigonometry(interpreter*);
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
        initialize_trigonometry(this);
    }

    void interpreter::execute(const std::vector<token>& tokens,
                              class stack<value>& stack,
                              hashmap<value>& local_variables,
                              std::istream& in,
                              std::ostream& out)
        throw(script_error, syntax_error)
    {
        const token_iterator end = tokens.end();

        for (token_iterator current = tokens.begin(); current < end;)
        {
            switch (current->type())
            {
                case token::type_lparen:
                case token::type_rparen:
                case token::type_rbrack:
                case token::type_lbrace:
                case token::type_rbrace:
                case token::type_keyword_else:
                {
                    std::stringstream ss;

                    ss << "unexpected " << current->type();

                    throw syntax_error(ss.str());
                }

                case token::type_int:
                    try
                    {
                        stack.push(string_to_int(current++->data()));
                    }
                    catch (std::length_error& e)
                    {
                        throw syntax_error(e.what());
                    }
                    break;

                case token::type_real:
                    try
                    {
                        stack.push(string_to_real(current++->data()));
                    }
                    catch (std::length_error& e)
                    {
                        throw syntax_error(e.what());
                    }
                    break;

                case token::type_string:
                    stack.push(current++->data());
                    break;

                case token::type_lbrack:
                    stack.push(parse_list(++current, end));
                    break;

                case token::type_keyword_if:
                    parse_if(
                            ++current,
                            end,
                            *this,
                            stack,
                            local_variables,
                            in,
                            out
                    );
                    break;

                case token::type_keyword_while:
                    parse_while(
                            ++current,
                            end,
                            *this,
                            stack,
                            local_variables,
                            in,
                            out
                    );
                    break;

                case token::type_keyword_to:
                    if (++current >= end || !current->is(token::type_word))
                    {
                        throw syntax_error("missing variable name after `to'");
                    }
                    else if (stack.size() < 1)
                    {
                        throw script_error("missing variable value");
                    }
                    local_variables.insert(current++->data(), stack.back());
                    stack.pop();
                    break;

                case token::type_word:
                {
                    const std::string& id = current++->data();
                    hashmap<value>::entry* e1 = local_variables.find(id);
                    hashmap<std::vector<function> >::entry* e2;

                    if (e1)
                    {
                        stack << e1->value;
                    }
                    else if ((e2 = m_functions.find(id)))
                    {
                        bool found = false;

                        for (auto& function : e2->value)
                        {
                            if (function.signature().test(stack))
                            {
                                found = true;
                                function.invoke(*this, stack, local_variables, in, out);
                                break;
                            }
                        }
                        if (!found)
                        {
                            throw script_error(
                                    "signature of function `"
                                    + id
                                    + "' does not match with given stack"
                            );
                        }
                    } else {
                        throw script_error(
                                "undefined function `"
                                + id
                                + "'"
                        );
                    }
                    break;
                }

                case token::type_colon:
                {
                    std::string name;
                    class signature signature;
                    std::vector<token> body;

                    if (++current < end && current->is(token::type_word))
                    {
                        name = current++->data();
                    }
                    if (current < end && current->is(token::type_lparen))
                    {
                        signature = parse_function_signature(++current, end);
                    }
                    body = parse_block(current, end);
                    if (name.empty())
                    {
                        stack.push(function(signature, body));
                    } else {
                        register_function(name, signature, body);
                    }
                }
            }
        }
    }

    void interpreter::register_function(const std::string& name,
                                        const class signature& signature,
                                        function::callback callback)
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
                                        function::callback callback)
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
            case token::type_int:
                try
                {
                    return string_to_int(token.data());
                }
                catch (std::length_error& e)
                {
                    throw syntax_error(e.what());
                }

            case token::type_real:
                try
                {
                    return string_to_real(token.data());
                }
                catch (std::length_error& e)
                {
                    throw syntax_error(e.what());
                }

            case token::type_string:
                return token.data();

            case token::type_lbrack:
                return parse_list(current, end);

            case token::type_colon:
            {
                class signature signature;

                if (current < end && current->is(token::type_lparen))
                {
                    signature = parse_function_signature(++current, end);
                }

                return function(signature, parse_block(current, end));
            }

            default:
            {
                std::stringstream ss;

                ss << "unexpected " << token.type() << "; missing value";

                throw syntax_error(ss.str());
            }
        }
    }

    static std::vector<value> parse_list(token_iterator& current,
                                         const token_iterator& end)
    {
        std::vector<value> list;

        while (current < end && !current->is(token::type_rbrack))
        {
            list.push_back(parse_value(current, end));
        }
        if (current++ >= end)
        {
            throw syntax_error("unterminated list: missing `]'");
        }

        return list;
    }

    static signature parse_function_signature(token_iterator& current,
                                              const token_iterator& end)
    {
        std::vector<signature::entry> parameter_types;
        std::vector<signature::entry> return_types;
        bool in_parameters = true;

        while (current < end && !current->is(token::type_rparen))
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
                else if (!id.compare("num"))
                {
                    entry = signature::type_num;
                }
                else if (!id.compare("int"))
                {
                    entry = signature::type_int;
                }
                else if (!id.compare("real"))
                {
                    entry = signature::type_real;
                }
                else if (!id.compare("ratio"))
                {
                    entry = signature::type_ratio;
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
        if (current++ >= end)
        {
            throw syntax_error("unterminated function signature: missing `)'");
        }

        return signature(parameter_types, return_types);
    }

    static std::vector<token> parse_block(token_iterator& current,
                                          const token_iterator& end)
    {
        std::vector<token> tokens;
        unsigned int counter = 1;

        if (current >= end || !current->is(token::type_lbrace))
        {
            throw syntax_error("missing block");
        }
        ++current;
        while (current < end)
        {
            const class token& token = *current++;

            if (token.is(token::type_lbrace))
            {
                ++counter;
            }
            else if (token.is(token::type_rbrace) && --counter == 0)
            {
                break;
            }
            tokens.push_back(token);
        }
        if (counter > 0)
        {
            throw syntax_error("unterminated block: missing `}'");
        }

        return tokens;
    }

    static void skip_block(token_iterator& current,
                           const token_iterator& end)
    {
        unsigned int counter = 1;

        if (current >= end || !current->is(token::type_lbrace))
        {
            throw syntax_error("missing block");
        }
        for (++current; current < end && counter > 0; ++current)
        {
            if (current->is(token::type_lbrace))
            {
                ++counter;
            }
            else if (current->is(token::type_rbrace))
            {
                --counter;
            }
        }
        if (counter > 0)
        {
            throw syntax_error("unterminated block: missing `}'");
        }
    }

    static void parse_if(token_iterator& current,
                         const token_iterator& end,
                         class interpreter& interpreter,
                         class stack<value>& stack,
                         hashmap<value>& local_variables,
                         std::istream& in,
                         std::ostream& out)
    {
        token_iterator begin = current;
        bool condition;

        while (current < end && !current->is(token::type_lbrace))
        {
            ++current;
        }
        interpreter.execute(
                std::vector<token>(begin, current),
                stack,
                local_variables,
                in,
                out
        );
        if (stack.empty() || !stack[stack.size() - 1].is(value::type_bool))
        {
            throw script_error("`if' statement is missing condition");
        }
        condition = stack[stack.size() - 1].as_bool();
        stack.pop();
        if (condition)
        {
            interpreter.execute(
                    parse_block(current, end),
                    stack,
                    local_variables,
                    in,
                    out
            );
            if (current < end && current->is(token::type_keyword_else))
            {
                skip_block(++current, end);
            }
        } else {
            skip_block(current, end);
            if (current < end && current->is(token::type_keyword_else))
            {
                interpreter.execute(
                        parse_block(++current, end),
                        stack,
                        local_variables,
                        in,
                        out
                );
            }
        }
    }

    static void parse_while(token_iterator& current,
                            const token_iterator& end,
                            class interpreter& interpreter,
                            class stack<value>& stack,
                            hashmap<value>& local_variables,
                            std::istream& in,
                            std::ostream& out)
    {
        token_iterator begin = current;
        std::vector<token> condition;
        std::vector<token> block;

        while (current < end && !current->is(token::type_lbrace))
        {
            ++current;
        }
        condition.assign(begin, current);
        block = parse_block(current, end);
        do
        {
            bool result;

            interpreter.execute(condition, stack, local_variables, in, out);
            if (stack.empty() || !stack[stack.size() - 1].is(value::type_bool))
            {
                throw script_error("`while' statement is missing condition");
            }
            result = stack[stack.size() - 1].as_bool();
            stack.pop();
            if (!result)
            {
                return;
            }
            interpreter.execute(block, stack, local_variables, in, out);
        }
        while (true);
    }
}
