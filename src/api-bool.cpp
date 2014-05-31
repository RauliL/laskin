#include "interpreter.hpp"
#include "value.hpp"

namespace laskin
{
    /**
     * true()
     *
     * Returns boolean value true.
     */
    BUILT_IN_FUNCTION(func_true)
    {
        stack.push_back(value(true));
    }

    /**
     * false()
     *
     * Returns boolean value false.
     */
    BUILT_IN_FUNCTION(func_false)
    {
        stack.push_back(value(false));
    }

    /**
     * &(bool bool : bool)
     *
     * Logical AND. Returns true if both boolean values are true.
     */
    BUILT_IN_FUNCTION(func_and)
    {
        const bool a = stack[stack.size() - 2].as_bool();
        const bool b = stack[stack.size() - 1].as_bool();

        stack.pop_back();
        stack.pop_back();
        stack.push_back(value(a && b));
    }

    /**
     * &(bool bool : bool)
     *
     * Logical OR. Returns true if either boolean value is true.
     */
    BUILT_IN_FUNCTION(func_or)
    {
        const bool a = stack[stack.size() - 2].as_bool();
        const bool b = stack[stack.size() - 1].as_bool();

        stack.pop_back();
        stack.pop_back();
        stack.push_back(value(a || b));
    }

    /**
     * ^(bool bool : bool)
     *
     * Exclusive OR.
     */
    BUILT_IN_FUNCTION(func_xor)
    {
        const bool a = stack[stack.size() - 2].as_bool();
        const bool b = stack[stack.size() - 1].as_bool();

        stack.pop_back();
        stack.pop_back();
        stack.push_back(a != b && (a || b));
    }

    /**
     * !(bool : bool)
     *
     * Negates given boolean value.
     */
    BUILT_IN_FUNCTION(func_not)
    {
        const bool a = stack[stack.size() - 1].as_bool();

        stack.pop_back();
        stack.push_back(value(!a));
    }

    namespace internal
    {
        void initialize_bool(interpreter* i)
        {
            i->register_function("true", ":b", func_true);
            i->register_function("false", ":b", func_false);

            i->register_function("and", "bb:b", func_and);
            i->register_function("or", "bb:b", func_or);
            i->register_function("xor", "bb:b", func_xor);
            i->register_function("not", "b:b", func_not);
        }
    }
}
