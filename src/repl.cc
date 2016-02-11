#include <iostream>
#include <string>
#include "eval.hh"
#include "builtins.hh"
#include "parser.hh"
#include "scheme_types.hh"

int main()
{
    auto env = standardEnvironment();

    do {
        try {
            std::cout << " * " << std::flush;
            SchemeExpr expr;
            if (readSchemeExpr(std::cin, expr)) {
                std::cout << eval(expr, env) << std::endl;
            }
        } catch (const scheme_error& e) {
            std::cerr << "error: " << e.what() << std::endl;
        }
    } while (std::cin);
}
