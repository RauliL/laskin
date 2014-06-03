#include "interpreter.hpp"
#include "script.hpp"
#include "value.hpp"
#include <sstream>

namespace laskin
{
    static value parse_literal(script::const_iterator&,
                               const script::const_iterator&);
    static value parse_list_literal(script::const_iterator&,
                                    const script::const_iterator&);
    static signature parse_function_signature(script::const_iterator&,
                                              const script::const_iterator&);
    static script parse_block(script::const_iterator&,
                              const script::const_iterator&);
    static void parse_if(interpreter&,
                         stack<value>&,
                         hashmap<value>&,
                         std::istream&,
                         std::ostream&,
                         script::const_iterator&,
                         const script::const_iterator&);
    static void parse_case(interpreter&,
                           stack<value>&,
                           hashmap<value>&,
                           std::istream&,
                           std::ostream&,
                           script::const_iterator&,
                           const script::const_iterator&);
    static void parse_while(interpreter&,
                            stack<value>&,
                            hashmap<value>&,
                            std::istream&,
                            std::ostream&,
                            script::const_iterator&,
                            const script::const_iterator&);

    script::script(const std::vector<token>& tokens)
        : m_tokens(tokens) {}

    script::script(const_iterator& first, const_iterator& last)
        : m_tokens(first, last) {}

    script::script(const script& that)
        : m_tokens(that.m_tokens) {}

    void script::execute(class interpreter& interpreter,
                         stack<value>& data,
                         hashmap<value>& locals,
                         std::istream& in,
                         std::ostream& out) const
        throw(error)
    {
        const const_iterator end = m_tokens.end();

        for (const_iterator current = m_tokens.begin(); current < end;)
        {
            switch (current->type())
            {
                case token::type_lparen:
                case token::type_rparen:
                case token::type_rbrack:
                case token::type_lbrace:
                case token::type_rbrace:
                case token::type_kw_else:
                {
                    std::stringstream ss;

                    ss << "unexpected " << current->type();

                    throw error(error::type_syntax, ss.str());
                }

                case token::type_int:
                case token::type_real:
                case token::type_ratio:
                case token::type_string:
                case token::type_lbrack:
                    data.push(parse_literal(current, end));
                    break;

                case token::type_kw_if:
                    parse_if(interpreter,
                             data,
                             locals,
                             in,
                             out,
                             ++current,
                             end);
                    break;

                case token::type_kw_case:
                    parse_case(interpreter,
                               data,
                               locals,
                               in,
                               out,
                               ++current,
                               end);
                    break;
                    
                case token::type_kw_while:
                    parse_while(interpreter,
                                data,
                                locals,
                                in,
                                out,
                                ++current,
                                end);
                    break;

                case token::type_kw_to:
                    if (++current >= end || !current->is(token::type_word))
                    {
                        throw error(
                                error::type_syntax,
                                "missing variable name after `to'"
                        );
                    }
                    else if (data.size() < 1)
                    {
                        throw error(
                                error::type_syntax,
                                "missing variable value"
                        );
                    }
                    locals.insert(current++->data(), data.back());
                    data.pop();
                    break;

                case token::type_colon:
                {
                    std::string name;
                    class signature signature;
                    script body;

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
                        data.push(function(signature, body));
                    } else {
                        interpreter.register_function(name, signature, body);
                    }
                    break;
                }

                case token::type_word:
                {
                    const std::string& id = current++->data();
                    hashmap<value>::entry* e = locals.find(id);

                    if (e)
                    {
                        data.push(e->value);
                    } else {
                        interpreter.execute(id, data, locals, in, out);
                    }
                }
            }
        }
    }

    script& script::assign(const script& that)
    {
        m_tokens = that.m_tokens;

        return *this;
    }

    script& script::assign(const_iterator& first, const_iterator& last)
    {
        m_tokens.assign(first, last);

        return *this;
    }

    static value parse_literal(script::const_iterator& current,
                               const script::const_iterator& end)
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
                    throw error(error::type_syntax, e.what());
                }

            case token::type_real:
                try
                {
                    return string_to_real(token.data());
                }
                catch (std::length_error& e)
                {
                    throw error(error::type_syntax, e.what());
                }

            case token::type_ratio:
                try
                {
                    const std::string& source = token.data();
                    const std::string::size_type pos = source.find('/');

                    if (pos == std::string::npos)
                    {
                        throw error(
                                error::type_syntax,
                                "rational number is missing denominator"
                        );
                    }
                    
                    
                    return ratio(
                            string_to_int(source.substr(0, pos)),
                            string_to_int(source.substr(pos + 1))
                    );
                }
                catch (std::length_error& e)
                {
                    throw error(error::type_syntax, e.what());
                }

            case token::type_string:
                return token.data();

            case token::type_lbrack:
                return parse_list_literal(current, end);

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

                ss << "unexpected "
                   << token.type()
                   << "; missing value literal";

                throw error(error::type_syntax, ss.str());
            }
        }
    }

    static value parse_list_literal(script::const_iterator& current,
                                    const script::const_iterator& end)
    {
        std::vector<value> list;

        while (current < end && !current->is(token::type_rbrack))
        {
            list.push_back(parse_literal(current, end));
        }
        if (current++ >= end)
        {
            throw error(
                    error::type_syntax,
                    "unterminated list literal: missing `]'"
            );
        }

        return list;
    }

    static signature parse_function_signature(script::const_iterator& current,
                                              const script::const_iterator& end)
    {
        std::vector<signature::entry> parameter_types;
        std::vector<signature::entry> return_types;
        bool in_parameters = true;

        while (current < end && !current->is(token::type_rparen))
        {
            if (current->is(token::type_word))
            {
                const std::string& id = current++->data();

                if (id.compare("--"))
                {
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
                        throw error(
                                error::type_syntax,
                                "unknown type: `"
                                + id
                                + "'"
                        );
                    }
                    if (in_parameters)
                    {
                        parameter_types.push_back(entry);
                    } else {
                        return_types.push_back(entry);
                    }
                }
                else if (in_parameters)
                {
                    in_parameters = false;
                } else {
                    throw error(
                            error::type_syntax,
                            "multiple `--' found in function signature"
                    );
                }
            } else {
                std::stringstream ss;

                ss << "unexpected "
                   << current->type()
                   << "; missing type signature";

                throw error(error::type_syntax, ss.str());
            }
        }
        if (current++ >= end)
        {
            throw error(
                    error::type_syntax,
                    "unterminated function signature: missing `)'"
            );
        }

        return signature(parameter_types, return_types);
    }

    static script parse_block(script::const_iterator& current,
                              const script::const_iterator& end)
    {
        std::vector<token> tokens;
        unsigned int counter = 1;

        if (current >= end || !current->is(token::type_lbrace))
        {
            throw error(error::type_syntax, "missing block");
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
            throw error(error::type_syntax, "unterminated block: missing `}'");
        }

        return script(tokens);
    }

    static void skip_block(script::const_iterator& current,
                           const script::const_iterator& end)
    {
        unsigned int counter = 1;

        if (current >= end || !current->is(token::type_lbrace))
        {
            throw error(error::type_syntax, "missing block");
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
            throw error(error::type_syntax, "unterminated block: missing `}'");
        }
    }

    static void parse_if(class interpreter& interpreter,
                         stack<value>& data,
                         hashmap<value>& locals,
                         std::istream& in,
                         std::ostream& out,
                         script::const_iterator& current,
                         const script::const_iterator& end)
    {
        script::const_iterator begin = current;
        bool condition;

        while (current < end && !current->is(token::type_lbrace))
        {
            ++current;
        }
        script(begin, current).execute(interpreter, data, locals, in, out);
        if (data.empty() || !data.back().is(value::type_bool))
        {
            throw error(
                    error::type_syntax,
                    "`if' statement is missing condition"
            );
        }
        condition = data.back().as_bool();
        data.pop();
        if (condition)
        {
            parse_block(current, end).execute(
                    interpreter,
                    data,
                    locals,
                    in,
                    out
            );
            if (current < end && current->is(token::type_kw_else))
            {
                skip_block(++current, end);
            }
        } else {
            skip_block(current, end);
            if (current < end && current->is(token::type_kw_else))
            {
                parse_block(++current, end).execute(
                        interpreter,
                        data,
                        locals,
                        in,
                        out
                );
            }
        }
    }

    static void parse_case(class interpreter& interpreter,
                           stack<value>& data,
                           hashmap<value>& locals,
                           std::istream& in,
                           std::ostream& out,
                           script::const_iterator& current,
                           const script::const_iterator& end)
    {
        script block = parse_block(current, end);
        script::const_iterator condition_begin = block.begin();
        script::const_iterator block_current = condition_begin;
        script::const_iterator block_end = block.end();

        while (block_current < block_end)
        {
            if (block_current->is(token::type_lbrace))
            {
                bool result;

                script(condition_begin, block_current).execute(
                        interpreter,
                        data,
                        locals,
                        in,
                        out
                );
                if (data.empty() || !data.back().is(value::type_bool))
                {
                    throw error(
                            error::type_syntax,
                            "`case' statement is missing condition"
                    );
                }
                result = data.back().as_bool();
                data.pop();
                if (result)
                {
                    parse_block(block_current, block_end).execute(
                            interpreter,
                            data,
                            locals,
                            in,
                            out
                    );
                    return;
                } else {
                    skip_block(block_current, block_end);
                    condition_begin = block_current;
                }
            }
            else if (block_current->is(token::type_kw_else))
            {
                parse_block(++block_current, block_end).execute(
                        interpreter,
                        data,
                        locals,
                        in,
                        out
                );
                return;
            } else {
                ++block_current;
            }
        }

        throw error(error::type_syntax, "no conditions in `case' statement");
    }

    static void parse_while(class interpreter& interpreter,
                            stack<value>& data,
                            hashmap<value>& locals,
                            std::istream& in,
                            std::ostream& out,
                            script::const_iterator& current,
                            const script::const_iterator& end)
    {
        script condition;
        script body;

        if (current >= end)
        {
            throw error(
                    error::type_syntax,
                    "`while' statement is missing condition"
            );
        }
        else if (!current->is(token::type_lbrace))
        {
            script::const_iterator begin = current;

            while (current < end && !current->is(token::type_lbrace))
            {
                ++current;
            }
            condition.assign(begin, current);
        }
        body = parse_block(current, end);
        do
        {
            if (!condition.empty())
            {
                bool result;

                condition.execute(interpreter, data, locals, in, out);
                if (data.empty() || !data.back().is(value::type_bool))
                {
                    throw error(
                            error::type_syntax,
                            "`while' statement is missing condition"
                    );
                }
                result = data.back().as_bool();
                data.pop();
                if (!result)
                {
                    return;
                }
            }
            body.execute(interpreter, data, locals, in, out);
        }
        while (true);
    }
}
