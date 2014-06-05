#include "interpreter.hpp"
#include "script.hpp"
#include "value.hpp"
#include <fstream>
#include <sstream>
#include <unistd.h>

using namespace laskin;

static void count_open_braces(stack<char>&, const std::string&);

static void cmd_peek(interpreter&, stack<value>&);
static void cmd_quit(interpreter&, stack<value>&);
static void cmd_stack(interpreter&, stack<value>&);

struct cli_command
{
    const char* name;
    const char* name_shortcut;
    void (*callback)(interpreter&, stack<laskin::value>&);
};

static const cli_command cli_command_list[] =
{
    {"peek", "p", cmd_peek},
    {"quit", "q", cmd_quit},
    {"stack", "s", cmd_stack},
    {0, 0, 0}
};

int main(int argc, char** argv)
{
    class interpreter interpreter;
    stack<value> data;
    hashmap<value> locals;

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
                    if (!data.empty())
                    {
                        data.clear();
                    }
                    if (!locals.empty())
                    {
                        locals.clear();
                    }
                    script::scan(is).execute(
                            interpreter,
                            data,
                            locals,
                            std::cin,
                            std::cout
                    );
                }
                catch (error& e)
                {
                    is.close();
                    if (e.is(error::type_exit))
                    {
                        std::exit(EXIT_SUCCESS);
                    } else {
                        std::cerr << "error: " << e.message() << std::endl;
                        std::exit(EXIT_FAILURE);
                    }
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
        int line_counter = 0;
        std::string source;
        stack<char> open_braces;

        while (std::cin.good())
        {
            std::string line;

            std::cout << "laskin:"
                      << ++line_counter
                      << ":"
                      << data.size()
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
                for (int i = 0; cli_command_list[i].name; ++i)
                {
                    if (!line.compare(cli_command_list[i].name)
                        || !line.compare(cli_command_list[i].name_shortcut))
                    {
                        found = true;
                        cli_command_list[i].callback(interpreter, data);
                        break;
                    }
                }
                if (!found)
                {
                    std::cout << "unrecognized command" << std::endl;
                }
            } else {
                count_open_braces(open_braces, line);
                source.append(line).append(1, '\n');
                if (open_braces.empty())
                {
                    std::stringstream ss;

                    ss.str(source);
                    source.clear();
                    try
                    {
                        script::scan(ss).execute(
                                interpreter,
                                data,
                                locals,
                                std::cin,
                                std::cout
                        );
                    }
                    catch (error& e)
                    {
                        if (e.is(error::type_exit))
                        {
                            std::exit(EXIT_SUCCESS);
                        } else {
                            std::cout << "error: " << e.message() << std::endl;
                        }
                    }
                }
            }
        }
    } else {
        try
        {
            script::scan(std::cin).execute(
                    interpreter,
                    data,
                    locals,
                    std::cin,
                    std::cout
            );
        }
        catch (error& e)
        {
            if (!e.is(error::type_exit))
            {
                std::cerr << "error: " << e.message() << std::endl;
                std::exit(EXIT_FAILURE);
            }
        }
    }

    return EXIT_SUCCESS;
}

static void count_open_braces(stack<char>& open_braces, const std::string& s)
{
    for (std::string::size_type i = 0; i < s.length(); ++i)
    {
        switch (s[i])
        {
            case '#':
                return;

            case '(':
                open_braces.push(')');
                break;

            case '[':
                open_braces.push(']');
                break;

            case '{':
                open_braces.push('}');
                break;

            case ')':
            case ']':
            case '}':
                if (!open_braces.empty() && open_braces.back() == s[i])
                {
                    open_braces.pop();
                }
                break;

            case '"':
            case '\'':
            {
                const char separator = s[i++];

                while (i < s.length())
                {
                    if (s[i] == separator)
                    {
                        break;
                    }
                    else if (s[i] == '\\' && i + 1 < s.length())
                    {
                        i += 2;
                    } else {
                        ++i;
                    }
                }
            }
        }
    }
}

static void cmd_peek(class interpreter& interpreter, stack<value>& data)
{
    if (data.empty())
    {
        std::cout << "stack is empty" << std::endl;
    } else {
        std::cout << data.back() << std::endl;
    }
}

static void cmd_quit(class interpreter& interpreter, stack<value>& data)
{
    std::exit(EXIT_SUCCESS);
}

static void cmd_stack(class interpreter& interpreter, stack<value>& data)
{
    for (stack<value>::size_type i = 0; i < data.size(); ++i)
    {
        if (i > 0)
        {
            std::cout << ' ';
        }
        std::cout << data[i];
    }
    std::cout << std::endl;
}
