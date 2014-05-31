#include "interpreter.hpp"
#include "value.hpp"

namespace laskin
{
    /**
     * length(list : integer)
     *
     * Returns the number of elements stored in the list.
     */
    BUILT_IN_FUNCTION(func_length)
    {
        const value a = stack[stack.size() - 1];

        stack.pop_back();
        stack.push_back(value(static_cast<integer>(a.as_list().size())));
    }

    /**
     * empty?(list : bool)
     *
     * Returns true if list is empty.
     */
    BUILT_IN_FUNCTION(func_is_empty)
    {
        const value a = stack[stack.size() - 1];

        stack.pop_back();
        stack.push_back(value(a.as_list().empty()));
    }

    /**
     * each(list function)
     *
     * Iterates each element in the list and passes them to the given function.
     */
    BUILT_IN_FUNCTION(func_each)
    {
        const value a = stack[stack.size() - 2];
        const value b = stack[stack.size() - 1];
        const class function& function = b.as_function();

        stack.pop_back();
        stack.pop_back();
        for (auto& value : a.as_list())
        {
            stack.push_back(value);
            if (!function.signature().test(stack))
            {
                throw script_error("function signature mismatch");
            }
            function.invoke(interpreter, stack);
        }
    }

    /**
     * @(list integer : list)
     *
     * Retrieves value from the list at given index. Negative indexes count
     * from backwards.
     */
    BUILT_IN_FUNCTION(func_at)
    {
        const value a = stack[stack.size() - 2];
        const value b = stack[stack.size() - 1];
        const std::vector<value>& list = a.as_list();
        integer index = b.as_integer();

        stack.pop_back();
        stack.pop_back();
        if (index < 0)
        {
            index += list.size();
        }
        if (index < 0 || index >= static_cast<integer>(list.size()))
        {
            throw script_error("list index out of bounds");
        }
        stack.push_back(list[index]);
    }

    /**
     * +(list any : list)
     *
     * Inserts value to the end of the list and returns result.
     */
    BUILT_IN_FUNCTION(func_add)
    {
        std::vector<value> list = stack[stack.size() - 2].as_list();

        list.push_back(stack[stack.size() - 1]);
        stack.pop_back();
        stack.pop_back();
        stack.push_back(list);
    }

    namespace internal
    {
        void initialize_list(interpreter* i)
        {
            i->register_function("length", "l:i", func_length);

            // Testing functions.
            i->register_function("empty?", "l:b", func_is_empty);

            // Traversing functions.
            i->register_function("each", "lf", func_each);

            i->register_function("@", "li:?", func_at);
            i->register_function("+", "l?:l", func_add);
        }
    }
}