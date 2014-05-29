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
            stack.push_back(a.as_integer() + b.as_integer());
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
            stack.push_back(a.as_integer() - b.as_integer());
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
            stack.push_back(a.as_integer() * b.as_integer());
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
            const integer x = a.as_integer();
            const integer y = b.as_integer();

            if (y == 0)
            {
                throw script_error("division by zero");
            }
            stack.push_back(x / y);
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
        }
    }
}
