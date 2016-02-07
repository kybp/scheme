#include <algorithm>
#include <cctype>
#include <string>
#include <sstream>
#include <vector>
#include <boost/variant.hpp>
#include "scheme.hh"

std::vector<std::string> tokenize(const std::string string)
{
    std::vector<std::string> tokens;
    std::ostringstream currentToken;

    for (const auto& c : string) {
        if (c == '(' || c == ')') {
            if (!currentToken.str().empty()) {
                tokens.push_back(currentToken.str());
                currentToken.str("");
            }
            tokens.push_back(std::string{ c });
        } else if (std::isspace(c)) {
            if (!currentToken.str().empty()) {
                tokens.push_back(currentToken.str());
                currentToken.str("");
            }
        } else currentToken << c;
    }

    if (!currentToken.str().empty()) tokens.push_back(currentToken.str());

    return tokens;
}

bool isInteger(const std::string token)
{
    return std::all_of(token.begin(), token.end(), ::isdigit);
}

boost::variant<int, std::string> readToken(const std::string token)
{
    if (isInteger(token)) {
        return { std::atoi(token.c_str()) };
    } else { // Not accounting for lists yet
        return { token };
    }
}
