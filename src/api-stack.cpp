#include "interpreter.hpp"
#include "value.hpp"

namespace laskin
{
    /**
     * clear
     *
     * Clears entire stack.
     */
    BUILT_IN_FUNCTION(func_clear)
    {
        stack.clear();
    }

    /**
     * depth( : integer)
     *
     * Returns current depth of the stack.
     */
    BUILT_IN_FUNCTION(func_depth)
    {
        stack.push_back(static_cast<integer>(stack.size()));
    }

    /**
     * drop(any)
     *
     * Discards top-most value from the stack.
     */
    BUILT_IN_FUNCTION(func_drop)
    {
        stack.pop_back();
    }

    /**
     * dup(any : any any)
     *
     * Duplicates top-most value on the stack.
     */
    BUILT_IN_FUNCTION(func_dup)
    {
        stack.push_back(stack[stack.size() - 1]);
    }

    namespace internal
    {
        void initialize_stack(interpreter* i)
        {
            i->register_function("clear", "", func_clear);
            i->register_function("depth", ":i", func_depth);
            i->register_function("drop", "?", func_drop);
            i->register_function("dup", "?:??", func_dup);
        }
    }
}
