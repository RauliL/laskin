#include "interpreter.hpp"
#include "token.hpp"
#include "value.hpp"
#include <fstream>
#include <sstream>
#include <unistd.h>

static void repl_peek(laskin::interpreter&, laskin::stack<laskin::value>&);
static void repl_quit(laskin::interpreter&, laskin::stack<laskin::value>&);
static void repl_stack(laskin::interpreter&, laskin::stack<laskin::value>&);

struct repl_command
{
    const char* name;
    const char* name_shortcut;
    void (*callback)(laskin::interpreter&, laskin::stack<laskin::value>&);
};

static const repl_command repl_command_list[] =
{
    {"peek", "p", repl_peek},
    {"quit", "q", repl_quit},
    {"stack", "s", repl_stack},
    {NULL, NULL, NULL}
};

int main(int argc, char** argv)
{
    laskin::interpreter interpreter;
    laskin::stack<laskin::value> stack;
    laskin::hashmap<laskin::value> local_variables;

    interpreter.initialize();
    if (argc > 1)
    {
        for (int i = 1; i < argc; ++i)
        {
            std::ifstream is(argv[i], std::ios_base::in);

            if (is.good())
            {
                try
                {
                    std::vector<laskin::token> tokens = laskin::token::scan(is);

                    if (!tokens.empty())
                    {
                        if (!stack.empty())
                        {
                            stack.clear();
                        }
                        if (!local_variables.empty())
                        {
                            local_variables.clear();
                        }
                        interpreter.execute(tokens, stack, local_variables);
                    }
                }
                catch (laskin::error& e)
                {
                    std::cerr << "error: " << e.what() << std::endl;
                    is.close();
                    std::exit(EXIT_FAILURE);
                }
                is.close();
            } else {
                std::cerr << "unable to open file `"
                          << argv[i]
                          << "' for reading"
                          << std::endl;
                std::exit(EXIT_FAILURE);
            }
        }
    }
    else if (isatty(fileno(stdin)))
    {
        unsigned int line_counter = 1;
        std::string line;
        std::stringstream ss;

        do
        {
            std::cout << "laskin:"
                      << line_counter++
                      << ":"
                      << stack.size()
                      << "> ";
            if (!std::getline(std::cin, line))
            {
                break;
            }
            if (line.empty())
            {
                continue;
            }
            else if (line[0] == '\\')
            {
                bool found = false;

                line = line.substr(1);
                for (int i = 0; repl_command_list[i].name; ++i)
                {
                    if (!line.compare(repl_command_list[i].name)
                        || !line.compare(repl_command_list[i].name_shortcut))
                    {
                        found = true;
                        repl_command_list[i].callback(interpreter, stack);
                        break;
                    }
                }
                if (!found)
                {
                    std::cout << "unrecognized command" << std::endl;
                }
            } else {
                ss.str(line);
                try
                {
                    std::vector<laskin::token> tokens = laskin::token::scan(ss);

                    if (!tokens.empty())
                    {
                        interpreter.execute(tokens, stack, local_variables);
                    }
                }
                catch (laskin::error& e)
                {
                    std::cout << "error: " << e.what() << std::endl;
                }
            }
        }
        while (std::cin.good());
    } else {
        try
        {
            std::vector<laskin::token> tokens = laskin::token::scan(std::cin);

            if (!tokens.empty())
            {
                interpreter.execute(tokens, stack, local_variables);
            }
        }
        catch (laskin::error& e)
        {
            std::cerr << "error: " << e.what() << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }

    return EXIT_SUCCESS;
}

static void repl_peek(laskin::interpreter& interpreter,
                      laskin::stack<laskin::value>& stack)
{
    if (stack.empty())
    {
        std::cout << "stack is empty" << std::endl;
    } else {
        std::cout << stack[stack.size() - 1] << std::endl;
    }
}

static void repl_quit(laskin::interpreter& interpreter,
                      laskin::stack<laskin::value>& stack)
{
    std::exit(EXIT_SUCCESS);
}

static void repl_stack(laskin::interpreter& interpreter,
                       laskin::stack<laskin::value>& stack)
{
    for (std::size_t i = stack.size(); i > 0; --i)
    {
        if (i != stack.size())
        {
            std::cout << ' ';
        }
        std::cout << stack[i - 1];
    }
    std::cout << std::endl;
}
