#include "interpreter.hpp"
#include "value.hpp"

namespace laskin
{
    /**
     * cr()
     *
     * Outputs new line into standard output stream.
     */
    BUILT_IN_FUNCTION(func_cr)
    {
        std::cout << std::endl;
    }

    /**
     * print(any)
     *
     * Outputs given object into the standard output stream.
     */
    BUILT_IN_FUNCTION(func_print)
    {
        std::cout << stack[stack.size() - 1];
        stack.pop_back();
    }

    /**
     * println(any)
     *
     * Outputs given object into the standard output stream with new line.
     */
    BUILT_IN_FUNCTION(func_println)
    {
        std::cout << stack[stack.size() - 1] << std::endl;
        stack.pop_back();
    }

    /**
     * .(any)
     *
     * Outputs given value prefixed with a white space character into the
     * standard output stream.
     */
    BUILT_IN_FUNCTION(func_dot)
    {
        std::cout << ' ' << stack[stack.size() - 1];
        stack.pop_back();
    }

    namespace internal
    {
        void initialize_io(interpreter* i)
        {
            i->register_function("cr", "", func_cr);
            i->register_function("print", "?", func_print);
            i->register_function("println", "?", func_println);
            i->register_function(".", "?", func_dot);
        }
    }
}
