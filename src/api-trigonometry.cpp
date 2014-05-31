#include "interpreter.hpp"
#include "value.hpp"
#include <cmath>

namespace laskin
{
    /**
     * acos(number : real)
     *
     * Computes the principal value of the arc cosine of given number.
     */
    BUILT_IN_FUNCTION(func_acos)
    {
        const double a = stack[stack.size() - 1].as_real();

        stack[stack.size() - 1] = std::acos(a);
    }

    /**
     * asin(number : real)
     *
     * Computes the principal value of the arc sine of given number.
     */
    BUILT_IN_FUNCTION(func_asin)
    {
        const double a = stack[stack.size() - 1].as_real();

        stack[stack.size() - 1] = std::asin(a);
    }

    /**
     * atan(number : real)
     *
     * Computes the principal value of the arc tangent of given number.
     */
    BUILT_IN_FUNCTION(func_atan)
    {
        const double a = stack[stack.size() - 1].as_real();

        stack[stack.size() - 1] = std::atan(a);
    }

    /**
     * cos(number : real)
     *
     * Computes the cosine of given number.
     */
    BUILT_IN_FUNCTION(func_cos)
    {
        const double a = stack[stack.size() - 1].as_real();

        stack[stack.size() - 1] = std::cos(a);
    }

    /**
     * deg(number : real)
     *
     * Converts radians into degrees.
     */
    BUILT_IN_FUNCTION(func_deg)
    {
        const double a = stack[stack.size() - 1].as_real();

        stack[stack.size() - 1] = a * 180 / M_PI;
    }

    /**
     * rad(number : real)
     *
     * Converts degrees into radians.
     */
    BUILT_IN_FUNCTION(func_rad)
    {
        const double a = stack[stack.size() - 1].as_real();

        stack[stack.size() - 1] = a * M_PI / 180;
    }

    /**
     * sin(number : real)
     *
     * Computes the sine of given number.
     */
    BUILT_IN_FUNCTION(func_sin)
    {
        const double a = stack[stack.size() - 1].as_real();

        stack[stack.size() - 1] = std::sin(a);
    }

    /**
     * tan(number : real)
     *
     * Computes the tangent of given number.
     */
    BUILT_IN_FUNCTION(func_tan)
    {
        const double a = stack[stack.size() - 1].as_real();

        stack[stack.size() - 1] = std::tan(a);
    }

    namespace internal
    {
        void initialize_trigonometry(interpreter* i)
        {
            i->register_function("acos", "n:r", func_acos);
            i->register_function("asin", "n:r", func_asin);
            i->register_function("atan", "n:r", func_atan);
            i->register_function("cos", "n:r", func_cos);
            i->register_function("deg", "n:r", func_deg);
            i->register_function("rad", "n:r", func_rad);
            i->register_function("sin", "n:r", func_sin);
            i->register_function("tan", "n:r", func_tan);
        }
    }
}
