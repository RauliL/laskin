#include "interpreter.hpp"
#include "value.hpp"
#include <algorithm>

namespace laskin
{
    /**
     * length(string : int)
     *
     * Returns length of the string.
     */
    BUILT_IN_FUNCTION(func_length)
    {
        const std::string s = stack[stack.size() - 1].as_string();

        stack.pop();
        stack.push(value(static_cast<integer>(s.length())));
    }

    /**
     * empty?(string : bool)
     *
     * Returns true if the string is empty.
     */
    BUILT_IN_FUNCTION(func_is_empty)
    {
        const std::string s = stack[stack.size() - 1].as_string();

        stack.pop();
        stack.push(value(s.empty()));
    }

    /**
     * blank?(string : bool)
     *
     * Returns true if the string is empty or contains only white space
     * characters.
     */
    BUILT_IN_FUNCTION(func_is_blank)
    {
        const std::string s = stack[stack.size() - 1].as_string();

        stack.pop();
        if (s.empty())
        {
            stack.push(value(true));
            return;
        }
        for (auto c : s)
        {
            if (!std::isspace(c))
            {
                stack.push(value(false));
                return;
            }
        }
        stack.push(value(true));
    }

    /**
     * lower?(string : bool)
     *
     * Returns true if the string is all lower case characters. Empty strings
     * return false.
     */
    BUILT_IN_FUNCTION(func_is_lower)
    {
        const std::string s = stack[stack.size() - 1].as_string();

        stack.pop();
        if (s.empty())
        {
            stack.push(value(false));
            return;
        }
        for (auto c : s)
        {
            if (!std::islower(c))
            {
                stack.push(value(false));
                return;
            }
        }
        stack.push(value(true));
    }

    /**
     * upper?(string : bool)
     *
     * Returns true if the string is all upper case characters. Empty strings
     * return false.
     */
    BUILT_IN_FUNCTION(func_is_upper)
    {
        const std::string s = stack[stack.size() - 1].as_string();

        stack.pop();
        if (s.empty())
        {
            stack.push(value(false));
            return;
        }
        for (auto c : s)
        {
            if (!std::isupper(c))
            {
                stack.push(value(false));
                return;
            }
        }
        stack.push(value(true));
    }

    /**
     * lower(string : string)
     *
     * Converts string into lower case.
     */
    BUILT_IN_FUNCTION(func_lower)
    {
        std::string s = stack[stack.size() - 1].as_string();

        stack.pop();
        std::transform(s.begin(), s.end(), s.begin(), ::tolower);
        stack.push(value(s));
    }

    /**
     * upper(string : string)
     *
     * Converts string into lower case.
     */
    BUILT_IN_FUNCTION(func_upper)
    {
        std::string s = stack[stack.size() - 1].as_string();

        stack.pop();
        std::transform(s.begin(), s.end(), s.begin(), ::toupper);
        stack.push(value(s));
    }

    /**
     * reverse(string : string)
     *
     * Returns reversed copy of the string.
     */
    BUILT_IN_FUNCTION(func_reverse)
    {
        const value a = stack[stack.size() - 1];
        const std::string& s = a.as_string();

        stack.pop();
        stack.push(std::string(s.rbegin(), s.rend()));
    }

    /**
     * +(string string : string)
     *
     * Concatenates two strings into one.
     */
    BUILT_IN_FUNCTION(func_add)
    {
        const value a = stack[stack.size() - 2];
        const value b = stack[stack.size() - 1];

        stack.pop();
        stack.pop();
        stack.push(a.as_string() + b.as_string());
    }

    /**
     * @(string int : int)
     *
     * Retrieves character from the string at given index. Negative indexes
     * count from backwards.
     */
    BUILT_IN_FUNCTION(func_at)
    {
        const value a = stack[stack.size() - 2];
        const value b = stack[stack.size() - 1];
        const std::string& string = a.as_string();
        integer index = b.as_int();

        stack.pop();
        stack.pop();
        if (index < 0)
        {
            index += string.length();
        }
        if (index < 0 || index >= static_cast<integer>(string.length()))
        {
            throw script_error("string index out of bounds");
        }
        stack.push(static_cast<integer>(string[index]));
    }

    /**
     * @=(string int int : string)
     *
     * Sets character in the string at given index. Negative indexes count from
     * backwards.
     */
    BUILT_IN_FUNCTION(func_at_set)
    {
        const value a = stack[stack.size() - 3];
        const value b = stack[stack.size() - 2];
        const value c = stack[stack.size() - 1];
        std::string string = a.as_string();
        integer index = b.as_int();

        stack.pop();
        stack.pop();
        stack.pop();
        if (index < 0)
        {
            index += string.length();
        }
        if (index < 0 || index >= static_cast<integer>(string.length()))
        {
            throw script_error("string index out of bounds");
        }
        string[index] = static_cast<char>(c.as_int());
        stack.push(string);
    }

    namespace internal
    {
        void initialize_string(interpreter* i)
        {
            i->register_function("length", "s:i", func_length);

            // Testing functions.
            i->register_function("blank?", "s:b", func_is_blank);
            i->register_function("empty?", "s:b", func_is_empty);
            i->register_function("lower?", "s:b", func_is_lower);
            i->register_function("upper?", "s:b", func_is_upper);

            // Conversion functions.
            i->register_function("lower", "s:s", func_lower);
            i->register_function("upper", "s:s", func_upper);
            i->register_function("reverse", "s:s", func_reverse);

            i->register_function("+", "ss:s", func_add);
            i->register_function("@", "si:i", func_at);
            i->register_function("@=", "sii:s", func_at_set);
        }
    }
}
