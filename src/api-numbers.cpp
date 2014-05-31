#include "interpreter.hpp"
#include "value.hpp"

namespace laskin
{
    /**
     * +(number number : number)
     *
     * Performs addition on given numbers.
     */
    BUILT_IN_FUNCTION(func_add)
    {
        const value b = stack[stack.size() - 1];
        const value a = stack[stack.size() - 2];

        stack.pop_back();
        stack.pop_back();
        if (a.is(value::type_real) || b.is(value::type_real))
        {
            stack.push_back(a.as_real() + b.as_real());
        } else {
            stack.push_back(a.as_int() + b.as_int());
        }
    }

    /**
     * -(number number : number)
     *
     * Performs substraction on given numbers.
     */
    BUILT_IN_FUNCTION(func_sub)
    {
        const value b = stack[stack.size() - 1];
        const value a = stack[stack.size() - 2];

        stack.pop_back();
        stack.pop_back();
        if (a.is(value::type_real) || b.is(value::type_real))
        {
            stack.push_back(a.as_real() - b.as_real());
        } else {
            stack.push_back(a.as_int() - b.as_int());
        }
    }

    /**
     * *(number number : number)
     *
     * Performs multiplication on given numbers.
     */
    BUILT_IN_FUNCTION(func_mul)
    {
        const value b = stack[stack.size() - 1];
        const value a = stack[stack.size() - 2];

        stack.pop_back();
        stack.pop_back();
        if (a.is(value::type_real) || b.is(value::type_real))
        {
            stack.push_back(a.as_real() * b.as_real());
        } else {
            stack.push_back(a.as_int() * b.as_int());
        }
    }

    /**
     * /(number number : number)
     *
     * Performs division on the given numbers.
     */
    BUILT_IN_FUNCTION(func_div)
    {
        const value b = stack[stack.size() - 1];
        const value a = stack[stack.size() - 2];

        stack.pop_back();
        stack.pop_back();
        if (a.is(value::type_real) || b.is(value::type_real))
        {
            stack.push_back(a.as_real() / b.as_real());
        } else {
            const integer x = a.as_int();
            const integer y = b.as_int();

            if (y == 0)
            {
                throw script_error("division by zero");
            }
            stack.push_back(x / y);
        }
    }

    /**
     * ==(any any : bool)
     *
     * Returns true if the two values are equal, false otherwise.
     */
    BUILT_IN_FUNCTION(func_eq)
    {
        const value a = stack[stack.size() - 2];
        const value b = stack[stack.size() - 1];

        stack.pop_back();
        stack.pop_back();
        stack.push_back(a == b);
    }

    /**
     * !=(any any : bool)
     *
     * Returns true if the two values are not equal, false otherwise.
     */
    BUILT_IN_FUNCTION(func_ne)
    {
        const value a = stack[stack.size() - 2];
        const value b = stack[stack.size() - 1];

        stack.pop_back();
        stack.pop_back();
        stack.push_back(a != b);
    }

    /**
     * <(number number : bool)
     *
     * Less-than comparison between two numbers.
     */
    BUILT_IN_FUNCTION(func_lt)
    {
        const value a = stack[stack.size() - 2];
        const value b = stack[stack.size() - 1];

        stack.pop_back();
        stack.pop_back();
        if (a.is(value::type_real) || b.is(value::type_real))
        {
            stack.push_back(a.as_real() < b.as_real());
        } else {
            stack.push_back(a.as_int() < b.as_int());
        }
    }

    /**
     * >(number number : bool)
     *
     * Greater-than comparison between two numbers.
     */
    BUILT_IN_FUNCTION(func_gt)
    {
        const value a = stack[stack.size() - 2];
        const value b = stack[stack.size() - 1];

        stack.pop_back();
        stack.pop_back();
        if (a.is(value::type_real) || b.is(value::type_real))
        {
            stack.push_back(a.as_real() > b.as_real());
        } else {
            stack.push_back(a.as_int() > b.as_int());
        }
    }

    /**
     * <(number number : bool)
     *
     * Less-than-equals comparison between two numbers.
     */
    BUILT_IN_FUNCTION(func_lte)
    {
        const value a = stack[stack.size() - 2];
        const value b = stack[stack.size() - 1];

        stack.pop_back();
        stack.pop_back();
        if (a.is(value::type_real) || b.is(value::type_real))
        {
            stack.push_back(a.as_real() <= b.as_real());
        } else {
            stack.push_back(a.as_int() <= b.as_int());
        }
    }

    /**
     * >=(number number : bool)
     *
     * Greater-than-equals comparison between two numbers.
     */
    BUILT_IN_FUNCTION(func_gte)
    {
        const value a = stack[stack.size() - 2];
        const value b = stack[stack.size() - 1];

        stack.pop_back();
        stack.pop_back();
        if (a.is(value::type_real) || b.is(value::type_real))
        {
            stack.push_back(a.as_real() >= b.as_real());
        } else {
            stack.push_back(a.as_int() >= b.as_int());
        }
    }

    /**
     * max(number number : number)
     *
     * Returns maximum of the two given values.
     */
    BUILT_IN_FUNCTION(func_max)
    {
        const value a = stack[stack.size() - 2];
        const value b = stack[stack.size() - 1];

        stack.pop_back();
        stack.pop_back();
        if (a.is(value::type_real) || b.is(value::type_real))
        {
            const double x = a.as_real();
            const double y = b.as_real();

            stack.push_back(x > y ? x : y);
        } else {
            const integer x = a.as_int();
            const integer y = b.as_int();

            stack.push_back(x > y ? x : y);
        }
    }

    /**
     * min(number number : number)
     *
     * Returns minimum of the two given values.
     */
    BUILT_IN_FUNCTION(func_min)
    {
        const value a = stack[stack.size() - 2];
        const value b = stack[stack.size() - 1];

        stack.pop_back();
        stack.pop_back();
        if (a.is(value::type_real) || b.is(value::type_real))
        {
            const double x = a.as_real();
            const double y = b.as_real();

            stack.push_back(x < y ? x : y);
        } else {
            const integer x = a.as_int();
            const integer y = b.as_int();

            stack.push_back(x < y ? x : y);
        }
    }

    namespace internal
    {
        void initialize_numbers(interpreter* i)
        {
            i->register_function("+", "nn:n", func_add);
            i->register_function("-", "nn:n", func_sub);
            i->register_function("*", "nn:n", func_mul);
            i->register_function("/", "nn:n", func_div);

            // Comparison operators.
            i->register_function("==", "??:b", func_eq);
            i->register_function("!=", "??:b", func_ne);
            i->register_function("<", "nn:b", func_lt);
            i->register_function(">", "nn:b", func_gt);
            i->register_function("<=", "nn:b", func_lte);
            i->register_function(">=", "nn:b", func_gte);

            i->register_function("max", "nn:n", func_max);
            i->register_function("min", "nn:n", func_min);
        }
    }
}
