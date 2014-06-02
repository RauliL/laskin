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
        out << std::endl;
    }

    /**
     * emit(int)
     *
     * Outputs given integer as ASCII character into the standard output
     * stream.
     */
    BUILT_IN_FUNCTION(func_emit)
    {
        out << static_cast<char>(stack.back().as_int());
        stack.pop();
    }

    /**
     * print(any)
     *
     * Outputs given object into the standard output stream.
     */
    BUILT_IN_FUNCTION(func_print)
    {
        out << stack.back();
        stack.pop();
    }

    /**
     * println(any)
     *
     * Outputs given object into the standard output stream with new line.
     */
    BUILT_IN_FUNCTION(func_println)
    {
        out << stack.back() << std::endl;
        stack.pop();
    }

    /**
     * .(any)
     *
     * Outputs given value prefixed with a white space character into the
     * standard output stream.
     */
    BUILT_IN_FUNCTION(func_dot)
    {
        out << ' ' << stack.back();
        stack.pop();
    }

    namespace internal
    {
        void initialize_io(interpreter* i)
        {
            i->register_function("cr", "", func_cr);
            i->register_function("emit", "i", func_emit);
            i->register_function("print", "?", func_print);
            i->register_function("println", "?", func_println);
            i->register_function(".", "?", func_dot);
        }
    }
}
