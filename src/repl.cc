#include <fstream>
#include <iostream>
#include <string>
#include "eval.hh"
#include "builtins.hh"
#include "parser.hh"
#include "scheme_types.hh"

int main(int argc, char **argv)
{
    auto env = standardEnvironment();

    while (--argc) {
        std::ifstream file(*++argv);
        if (!file) std::cerr << "error: can't open " << *argv << std::endl;
        else {
            try {
                evalStream(file, env);   
            } catch (const scheme_error& e) {
                std::cerr << "error in '" << *argv << "': ";
                std::cerr << e.what() << std::endl;
            }
        }
    }

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
