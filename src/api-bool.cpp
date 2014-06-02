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
        stack.push(true);
    }

    /**
     * false()
     *
     * Returns boolean value false.
     */
    BUILT_IN_FUNCTION(func_false)
    {
        stack.push(false);
    }

    /**
     * and(bool bool : bool)
     *
     * Logical AND. Returns true if both boolean values are true.
     */
    BUILT_IN_FUNCTION(func_and)
    {
        value a, b;

        stack >> b >> a << value(a.as_bool() && b.as_bool());
    }

    /**
     * or(bool bool : bool)
     *
     * Logical OR. Returns true if either boolean value is true.
     */
    BUILT_IN_FUNCTION(func_or)
    {
        value a, b;

        stack >> b >> a << value(a.as_bool() || b.as_bool());
    }

    /**
     * xor(bool bool : bool)
     *
     * Exclusive OR.
     */
    BUILT_IN_FUNCTION(func_xor)
    {
        value a, b;

        stack >> a >> b
              << value(a.as_bool() != b.as_bool() && (a.as_bool() || b.as_bool()));
    }

    /**
     * not(bool : bool)
     *
     * Negates given boolean value.
     */
    BUILT_IN_FUNCTION(func_not)
    {
        value a;

        stack >> a << value(!a.as_bool());
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
