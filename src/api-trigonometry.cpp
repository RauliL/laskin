#include "interpreter.hpp"
#include "value.hpp"
#include <cmath>

namespace laskin
{
    /**
     * acos(num : real)
     *
     * Computes the principal value of the arc cosine of given number.
     */
    BUILT_IN_FUNCTION(func_acos)
    {
        stack.back() = std::acos(stack.back().as_real());
    }

    /**
     * asin(num : real)
     *
     * Computes the principal value of the arc sine of given number.
     */
    BUILT_IN_FUNCTION(func_asin)
    {
        stack.back() = std::asin(stack.back().as_real());
    }

    /**
     * atan(num : real)
     *
     * Computes the principal value of the arc tangent of given number.
     */
    BUILT_IN_FUNCTION(func_atan)
    {
        stack.back() = std::atan(stack.back().as_real());
    }

    /**
     * cos(num : real)
     *
     * Computes the cosine of given number.
     */
    BUILT_IN_FUNCTION(func_cos)
    {
        stack.back() = std::cos(stack.back().as_real());
    }

    /**
     * deg(num : real)
     *
     * Converts radians into degrees.
     */
    BUILT_IN_FUNCTION(func_deg)
    {
        stack.back() = stack.back().as_real() * 180 / M_PI;
    }

    /**
     * rad(num : real)
     *
     * Converts degrees into radians.
     */
    BUILT_IN_FUNCTION(func_rad)
    {
        stack.back() = stack.back().as_real() * M_PI / 180;
    }

    /**
     * sin(num : real)
     *
     * Computes the sine of given number.
     */
    BUILT_IN_FUNCTION(func_sin)
    {
        stack.back() = std::sin(stack.back().as_real());
    }

    /**
     * tan(num : real)
     *
     * Computes the tangent of given number.
     */
    BUILT_IN_FUNCTION(func_tan)
    {
        stack.back() = std::tan(stack.back().as_real());
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
