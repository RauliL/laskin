#ifndef LASKIN_DEFINES_HPP_GUARD
#define LASKIN_DEFINES_HPP_GUARD

#include <climits>
#include <cfloat>
#include "stack.hpp"

namespace laskin
{
    class function;
    class interpreter;
    class ratio;
    class signature;
    class token;
    class value;

    typedef signed long long int integer;
    typedef unsigned long long int uinteger;
    typedef double real;
}

#if defined(LLONG_MAX)
# define LASKIN_INTEGER_MAX LLONG_MAX
#else
# define LASKIN_INTEGER_MAX 0x7fffffffffffffff
#endif
#if defined(LLONG_MIN)
# define LASKIN_INTEGER_MIN LLONG_MIN
#else
# define LASKIN_INTEGER_MIN (-LASKIN_INTEGER_MAX - 1)
#endif
#if defined(DBL_MAX)
# define LASKIN_REAL_MAX DBL_MAX
#else
# define LASKIN_REAL_MAX 1.7976931348623157e+308
#endif
#if defined(DBL_MIN)
# define LASKIN_REAL_MIN DBL_MIN
#else
# define LASKIN_REAL_MIN 2.2250738585072014e-308
#endif

#define LASKIN_IS_IN_INTEGER_RANGE(x) \
    ((x) <= LASKIN_INTEGER_MAX && (x) >= LASKIN_INTEGER_MIN)

#define BUILT_IN_FUNCTION(Name) \
    static void Name(class laskin::interpreter& interpreter, \
                     class laskin::stack<value>& stack, \
                     hashmap<value>& local_variables, \
                     std::istream& in, \
                     std::ostream& out)

#endif /* !LASKIN_DEFINES_HPP_GUARD */
