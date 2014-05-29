#include "interpreter.hpp"
#include "token.hpp"
#include "value.hpp"
#include <fstream>
#include <sstream>
#include <unistd.h>

int main(int argc, char** argv)
{
    laskin::interpreter interpreter;
    std::deque<laskin::value> stack;

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
                        interpreter.execute(tokens, stack);
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
                // TODO: process repl command
            } else {
                ss.str(line);
                try
                {
                    std::vector<laskin::token> tokens = laskin::token::scan(ss);

                    if (!tokens.empty())
                    {
                        interpreter.execute(tokens, stack);
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
                interpreter.execute(tokens, stack);
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
