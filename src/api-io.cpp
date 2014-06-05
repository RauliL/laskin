#include "interpreter.hpp"
#include "value.hpp"

namespace laskin
{
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
     * nl()
     *
     * Outputs new line into standard output stream.
     */
    BUILT_IN_FUNCTION(func_nl)
    {
        out << std::endl;
    }

    /**
     * print(any)
     *
     * Outputs given object into the standard output stream with appended new
     * line.
     */
    BUILT_IN_FUNCTION(func_print)
    {
        out << stack.back() << std::endl;
        stack.pop();
    }

    namespace internal
    {
        void initialize_io(interpreter* i)
        {
            i->register_function("emit", "i", func_emit);
            i->register_function("nl", "", func_nl);
            i->register_function("print", "?", func_print);
        }
    }
}
