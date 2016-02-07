#include <iostream>
#include <string>
#include "scheme.hh"

int main()
{
    std::string line;
    SchemeEnvironment env;

    do {
        try {
            std::cout << " * " << std::flush;
            getline(std::cin, line);
            if (std::cin) std::cout << eval(parse(line), env) << std::endl;
        } catch (std::exception& e) {
            std::cerr << "error: " << e.what() << std::endl;
        }
    } while (std::cin);
}
