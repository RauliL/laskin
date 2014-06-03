#include "interpreter.hpp"
#include "value.hpp"
#include <cmath>

namespace laskin
{
    /**
     * +(num num : num)
     *
     * Performs addition on given numbers.
     */
    BUILT_IN_FUNCTION(func_add)
    {
        value a, b;

        stack >> b >> a;
        if (a.is(value::type_real) || b.is(value::type_real))
        {
            stack.push(a.as_real() + b.as_real());
        }
        else if (a.is(value::type_ratio))
        {
            if (b.is(value::type_ratio))
            {
                stack.push(a.as_ratio() + b.as_ratio());
            } else {
                stack.push(a.as_ratio() + b.as_int());
            }
        } else {
            stack.push(a.as_int() + b.as_int());
        }
    }

    /**
     * -(num num : num)
     *
     * Performs substraction on given numbers.
     */
    BUILT_IN_FUNCTION(func_sub)
    {
        value a, b;

        stack >> b >> a;
        if (a.is(value::type_real) || b.is(value::type_real))
        {
            stack.push(a.as_real() - b.as_real());
        }
        else if (a.is(value::type_ratio))
        {
            if (b.is(value::type_ratio))
            {
                stack.push(a.as_ratio() - b.as_ratio());
            } else {
                stack.push(a.as_ratio() - b.as_int());
            }
        } else {
            stack.push(a.as_int() - b.as_int());
        }
    }

    /**
     * *(num num : num)
     *
     * Performs multiplication on given numbers.
     */
    BUILT_IN_FUNCTION(func_mul)
    {
        value a, b;

        stack >> b >> a;
        if (a.is(value::type_real) || b.is(value::type_real))
        {
            stack.push(a.as_real() * b.as_real());
        }
        else if (a.is(value::type_ratio))
        {
            if (b.is(value::type_ratio))
            {
                stack.push(a.as_ratio() * b.as_ratio());
            } else {
                stack.push(a.as_ratio() * b.as_int());
            }
        } else {
            stack.push(a.as_int() * b.as_int());
        }
    }

    /**
     * /(num num : num)
     *
     * Performs division on the given numbers.
     */
    BUILT_IN_FUNCTION(func_div)
    {
        value a, b;

        stack >> b >> a;
        if (a.is(value::type_real) || b.is(value::type_real))
        {
            stack.push(a.as_real() / b.as_real());
        }
        else if (a.is(value::type_ratio))
        {
            if (b.is(value::type_ratio))
            {
                stack.push(a.as_ratio() / b.as_ratio());
            } else {
                stack.push(a.as_ratio() / b.as_int());
            }
        } else {
            const integer x = a.as_int();
            const integer y = b.as_int();

            if (y == 0)
            {
                throw script_error("division by zero");
            }
            else if (y > x)
            {
                stack.push(ratio(x, y));
            } else {
                stack.push(x / y);
            }
        }
    }

    /**
     * ==(any any : bool)
     *
     * Returns true if the two values are equal, false otherwise.
     */
    BUILT_IN_FUNCTION(func_eq)
    {
        value a, b;

        stack >> b >> a << value(a == b);
    }

    /**
     * !=(any any : bool)
     *
     * Returns true if the two values are not equal, false otherwise.
     */
    BUILT_IN_FUNCTION(func_ne)
    {
        value a, b;

        stack >> b >> a << value(a != b);
    }

    /**
     * <(num num : bool)
     *
     * Less-than comparison between two numbers.
     */
    BUILT_IN_FUNCTION(func_lt)
    {
        value a, b;

        stack >> b >> a;
        if (a.is(value::type_real) || b.is(value::type_real))
        {
            stack.push(a.as_real() < b.as_real());
        } else {
            stack.push(a.as_int() < b.as_int());
        }
    }

    /**
     * >(num num : bool)
     *
     * Greater-than comparison between two numbers.
     */
    BUILT_IN_FUNCTION(func_gt)
    {
        value a, b;

        stack >> b >> a;
        if (a.is(value::type_real) || b.is(value::type_real))
        {
            stack.push(a.as_real() > b.as_real());
        } else {
            stack.push(a.as_int() > b.as_int());
        }
    }

    /**
     * <(num num : bool)
     *
     * Less-than-equals comparison between two numbers.
     */
    BUILT_IN_FUNCTION(func_lte)
    {
        value a, b;

        stack >> b >> a;
        if (a.is(value::type_real) || b.is(value::type_real))
        {
            stack.push(a.as_real() <= b.as_real());
        } else {
            stack.push(a.as_int() <= b.as_int());
        }
    }

    /**
     * >=(num num : bool)
     *
     * Greater-than-equals comparison between two numbers.
     */
    BUILT_IN_FUNCTION(func_gte)
    {
        value a, b;

        stack >> b >> a;
        if (a.is(value::type_real) || b.is(value::type_real))
        {
            stack.push(a.as_real() >= b.as_real());
        } else {
            stack.push(a.as_int() >= b.as_int());
        }
    }

    /**
     * max(num num : num)
     *
     * Returns maximum of the two given values.
     */
    BUILT_IN_FUNCTION(func_max)
    {
        value a, b;

        stack >> b >> a;
        if (a.is(value::type_real) || b.is(value::type_real))
        {
            const real x = a.as_real();
            const real y = b.as_real();

            stack.push(x > y ? x : y);
        } else {
            const integer x = a.as_int();
            const integer y = b.as_int();

            stack.push(x > y ? x : y);
        }
    }

    /**
     * min(num num : num)
     *
     * Returns minimum of the two given values.
     */
    BUILT_IN_FUNCTION(func_min)
    {
        value a, b;

        stack >> b >> a;
        if (a.is(value::type_real) || b.is(value::type_real))
        {
            const real x = a.as_real();
            const real y = b.as_real();

            stack.push(x < y ? x : y);
        } else {
            const integer x = a.as_int();
            const integer y = b.as_int();

            stack.push(x < y ? x : y);
        }
    }

    /**
     * e( : real)
     *
     * Returns eulers number.
     */
    BUILT_IN_FUNCTION(func_e)
    {
        stack.push(M_E);
    }

    /**
     * inf( : real)
     *
     * Returns infinity.
     */
    BUILT_IN_FUNCTION(func_inf)
    {
        stack.push(INFINITY);
    }

    /**
     * pi( : real)
     *
     * Returns value of PI.
     */
    BUILT_IN_FUNCTION(func_pi)
    {
        stack.push(M_PI);
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

            // Constants.
            i->register_function("e", "", func_e);
            i->register_function("inf", "", func_inf);
            i->register_function("pi", "", func_pi);
        }
    }
}
