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
     * >list(all : list)
     *
     * Takes every value from the stack and returns them in a list.
     */
    BUILT_IN_FUNCTION(func_to_list)
    {
        std::vector<value> list(stack.begin(), stack.end());

        stack.clear();
        stack.push_back(value(list));
    }

    namespace internal
    {
        void initialize_list(interpreter* i)
        {
            i->register_function("length", "l:i", func_length);

            // Testing functions.
            i->register_function("empty?", "l:b", func_is_empty);

            i->register_function(">list", "*:l", func_to_list);
        }
    }
}
