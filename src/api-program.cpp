#include "interpreter.hpp"
#include "value.hpp"
#include <fstream>

namespace laskin
{
    /**
     * call(function)
     *
     * Calls function from the top of the stack with rest of the stack as
     * arguments.
     */
    BUILT_IN_FUNCTION(func_call)
    {
        const class value value = stack[stack.size() - 1];
        const class function& function = value.as_function();

        stack.pop_back();
        if (!function.signature().test(stack))
        {
            throw script_error("function signature mismatch");
        }
        function.invoke(interpreter, stack);
    }

    /**
     * exit()
     *
     * Terminates program execution.
     */
    BUILT_IN_FUNCTION(func_exit)
    {
        std::exit(EXIT_SUCCESS);
    }

    /**
     * include(string)
     *
     * Includes source code from given file name and executes it.
     */
    BUILT_IN_FUNCTION(func_include)
    {
        const std::string filename = stack[stack.size() - 1].as_string();
        std::ifstream in(filename, std::ios_base::in);

        stack.pop_back();
        if (in.good())
        {
            try
            {
                std::vector<token> tokens = token::scan(in);
                std::deque<value> new_stack;

                interpreter.execute(tokens, new_stack);
            }
            catch (error& e)
            {
                in.close();
                throw;
            }
            in.close();
        } else {
            throw script_error("unable to include file `" + filename + "'");
        }
    }

    namespace internal
    {
        void initialize_program(interpreter* i)
        {
            i->register_function("call", "f", func_call);
            i->register_function("exit", "", func_exit);
            i->register_function("include", "s", func_include);
        }
    }
}
