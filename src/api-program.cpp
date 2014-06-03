#include "interpreter.hpp"
#include "script.hpp"
#include "value.hpp"
#include <fstream>
#include <sstream>

namespace laskin
{
    /**
     * evaluate(string)
     *
     * Compiles given string into tokens and executes them.
     */
    BUILT_IN_FUNCTION(func_evaluate)
    {
        std::stringstream input;

        input.str(stack.back().as_string());
        stack.pop();
        script::scan(input).execute(
                interpreter,
                stack,
                local_variables,
                in,
                out
        );
    }

    /**
     * execute(function)
     *
     * Calls function from the top of the stack with rest of the stack as
     * arguments.
     */
    BUILT_IN_FUNCTION(func_execute)
    {
        const class value value = stack[stack.size() - 1];
        const class function& function = value.as_function();
        hashmap<class value> new_local_variables(local_variables);

        stack.pop();
        if (!function.signature().test(stack))
        {
            throw error(error::type_type, "function signature mismatch");
        }
        function.invoke(interpreter, stack, new_local_variables, in, out);
    }

    /**
     * exit()
     *
     * Terminates program execution.
     */
    BUILT_IN_FUNCTION(func_exit)
    {
        throw error(error::type_exit, "script exit");
    }

    /**
     * include(string)
     *
     * Includes source code from given file name and executes it.
     */
    BUILT_IN_FUNCTION(func_include)
    {
        const std::string filename = stack[stack.size() - 1].as_string();
        std::ifstream input(filename, std::ios_base::in);

        stack.pop();
        if (input.good())
        {
            try
            {
                laskin::stack<value> new_data;
                hashmap<value> new_locals;

                script::scan(input).execute(
                        interpreter,
                        new_data,
                        new_locals,
                        in,
                        out
                );
            }
            catch (error& e)
            {
                input.close();
                throw;
            }
            input.close();
        } else {
            throw error(
                    error::type_unknown,
                    "unable to include file `" + filename + "'"
            );
        }
    }

    /**
     * delete(string)
     *
     * Removes a local variable.
     */
    BUILT_IN_FUNCTION(func_delete)
    {
        local_variables.remove(stack.back().as_string());
        stack.pop();
    }

    namespace internal
    {
        void initialize_program(interpreter* i)
        {
            i->register_function("evaluate", "s", func_evaluate);
            i->register_function("execute", "f", func_execute);
            i->register_function("exit", "", func_exit);
            i->register_function("include", "s", func_include);
            i->register_function("delete", "s", func_delete);
        }
    }
}
