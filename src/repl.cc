#include <iostream>
#include <string>
#include "eval.hh"
#include "builtins.hh"
#include "parser.hh"
#include "scheme_types.hh"

int main()
{
    std::string line;
    auto env = standardEnvironment();

    do {
        try {
            std::cout << " * " << std::flush;
            getline(std::cin, line);
            if (std::cin) std::cout << eval(parse(line), env) << std::endl;
        } catch (const scheme_error& e) {
            std::cerr << "error: " << e.what() << std::endl;
        }
    } while (std::cin);
}
