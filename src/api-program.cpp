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
        hashmap<class value> new_local_variables(local_variables);

        stack.pop();
        if (!function.signature().test(stack))
        {
            throw script_error("function signature mismatch");
        }
        function.invoke(interpreter, stack, new_local_variables);
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

        stack.pop();
        if (in.good())
        {
            try
            {
                std::vector<token> tokens = token::scan(in);
                laskin::stack<value> new_stack;
                hashmap<value> new_local_variables;

                interpreter.execute(tokens, new_stack, new_local_variables);
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

    /**
     * get(string : any)
     *
     * Retrieves given local variable or throws an error if no such variable
     * exists.
     */
    BUILT_IN_FUNCTION(func_get)
    {
        const value a = stack[stack.size() - 1];
        hashmap<value>::entry* e = local_variables.find(a.as_string());

        stack.pop();
        if (e)
        {
            stack.push(e->value);
        } else {
            throw script_error("undefined local variable: `" + a.as_string() + "'");
        }
    }

    /**
     * set(any string)
     *
     * Sets a local variable.
     */
    BUILT_IN_FUNCTION(func_set)
    {
        value a, b;

        stack >> b >> a;
        local_variables.insert(b.as_string(), a);
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
            i->register_function("call", "f", func_call);
            i->register_function("exit", "", func_exit);
            i->register_function("include", "s", func_include);
            i->register_function("get", "s:?", func_get);
            i->register_function("set", "?s", func_set);
            i->register_function("delete", "s", func_delete);
        }
    }
}
