#include "interpreter.hpp"
#include "value.hpp"

namespace laskin
{
    /**
     * clear()
     *
     * Clears entire stack.
     */
    BUILT_IN_FUNCTION(func_clear)
    {
        stack.clear();
    }

    /**
     * depth(-- int)
     *
     * Returns current depth of the stack.
     */
    BUILT_IN_FUNCTION(func_depth)
    {
        stack.push(static_cast<integer>(stack.size()));
    }

    /**
     * drop(any)
     *
     * Discards top-most value from the stack.
     */
    BUILT_IN_FUNCTION(func_drop)
    {
        stack.pop();
    }

    /**
     * drop2(any any)
     *
     * Discards two top-most values from the stack.
     */
    BUILT_IN_FUNCTION(func_drop2)
    {
        stack.pop();
        stack.pop();
    }

    /**
     * dup(any -- any any)
     *
     * Duplicates top-most value on the stack.
     */
    BUILT_IN_FUNCTION(func_dup)
    {
        stack.push(stack[stack.size() - 1]);
    }

    /**
     * dup2(any any -- any any any any)
     *
     * Duplicates two top-most values on the stack.
     */
    BUILT_IN_FUNCTION(func_dup2)
    {
        value a, b;

        stack >> b >> a << a << b << a << b;
    }

    /**
     * nip(any any -- nip)
     *
     * Drops the first value and pushes the second value back into the stack.
     */
    BUILT_IN_FUNCTION(func_nip)
    {
        value a, b;

        stack >> b >> a << b;
    }

    /**
     * over(any any -- any any any)
     *
     * Copies second top-most value from the stack into top-most value of the
     * stack.
     */
    BUILT_IN_FUNCTION(func_over)
    {
        value a, b;

        stack >> b >> a << b << a << b;
    }

    /**
     * rot(any any any -- any any any)
     *
     * Rotates three top-most values on the stack.
     */
    BUILT_IN_FUNCTION(func_rot)
    {
        value a, b, c;

        stack >> c >> b >> a << c << b << a;
    }

    /**
     * swap(any any -- any any)
     *
     * Swaps positions of two top-most values on the stack.
     */
    BUILT_IN_FUNCTION(func_swap)
    {
        value a, b;

        stack >> b >> a << b << a;
    }

    /**
     * is-bool?(any -- bool)
     *
     * Returns true if given value is a boolean.
     */
    BUILT_IN_FUNCTION(func_is_bool)
    {
        value a;

        stack >> a << value(a.is(value::type_bool));
    }

    /**
     * is-num?(any -- bool)
     *
     * Returns true if given value is any type of number.
     */
    BUILT_IN_FUNCTION(func_is_num)
    {
        value a;

        stack >> a << value(
                a.is(value::type_int) ||
                a.is(value::type_real) ||
                a.is(value::type_ratio)
        );
    }

    /**
     * is-int?(any -- bool)
     *
     * Returns true if given value is integer.
     */
    BUILT_IN_FUNCTION(func_is_int)
    {
        value a;

        stack >> a << value(a.is(value::type_int));
    }

    /**
     * is-real?(any -- bool)
     *
     * Returns true if given value is real number.
     */
    BUILT_IN_FUNCTION(func_is_real)
    {
        value a;

        stack >> a << value(a.is(value::type_real));
    }

    /**
     * is-ratio?(any -- bool)
     *
     * Returns true if given value is rational number.
     */
    BUILT_IN_FUNCTION(func_is_ratio)
    {
        value a;

        stack >> a << value(a.is(value::type_ratio));
    }

    /**
     * is-string?(any -- bool)
     *
     * Returns true if given value is a string.
     */
    BUILT_IN_FUNCTION(func_is_string)
    {
        value a;

        stack >> a << value(a.is(value::type_string));
    }

    /**
     * is-list?(any -- bool)
     *
     * Returns true if given value is a list.
     */
    BUILT_IN_FUNCTION(func_is_list)
    {
        value a;

        stack >> a << value(a.is(value::type_list));
    }

    /**
     * is-function?(any -- bool)
     *
     * Returns true if given value is a function.
     */
    BUILT_IN_FUNCTION(func_is_function)
    {
        value a;

        stack >> a << value(a.is(value::type_function));
    }

    namespace internal
    {
        void initialize_stack(interpreter* i)
        {
            i->register_function("clear", "", func_clear);
            i->register_function("depth", ":i", func_depth);
            i->register_function("drop", "?", func_drop);
            i->register_function("drop2", "??", func_drop2);
            i->register_function("dup", "?:??", func_dup);
            i->register_function("dup2", "??:??", func_dup2);
            i->register_function("nip", "??:?", func_nip);
            i->register_function("over", "??:???", func_over);
            i->register_function("rot", "???:???", func_rot);
            i->register_function("swap", "??:??", func_swap);

            i->register_function("is-bool?", "?:b", func_is_bool);
            i->register_function("is-num?", "?:b", func_is_num);
            i->register_function("is-int?", "?:b", func_is_int);
            i->register_function("is-real?", "?:b", func_is_real);
            i->register_function("is-ratio?", "?:b", func_is_ratio);
            i->register_function("is-string?", "?:b", func_is_string);
            i->register_function("is-list?", "?:b", func_is_list);
            i->register_function("is-function?", "?:b", func_is_function);
        }
    }
}
