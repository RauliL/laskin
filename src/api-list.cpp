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

            i->register_function("+", "l?:l", func_add);
        }
    }
}
