#include <algorithm> // for std::transform, std::all_of
#include <cctype>    // for std::isdigit
#include <deque>
#include <string>
#include <sstream>
#include <vector>
#include <boost/variant.hpp>
#include "parser.hh"
#include "scheme.hh"

std::deque<std::string> tokenize(const std::string string)
{
    std::deque<std::string> tokens;
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
    auto begin = token.begin();
    auto end   = token.end();
    if (begin != end && *begin == '-') ++begin;
    if (begin == end) return false;
    return std::all_of(begin, end, ::isdigit);
}

SchemeExpr readFromTokens(std::deque<std::string>& tokens)
{
    if (tokens.empty()) throw scheme_error("unexpected EOF while reading");

    std::string token = tokens.front();
    tokens.pop_front();
    if (token == "(") {
        SchemeList list;
        while (tokens.front() != ")") {
            list.push_back(readFromTokens(tokens));
        }
        tokens.pop_front();     // remove ")"
        return list;
    } else if (token == ")") {
        throw scheme_error("unexpected ')'");
    } else if (isInteger(token)) {
        return std::atoi(token.c_str());
    } else if (token == "#t") {
        return true;
    } else if (token == "#f") {
        return false;
    } else {
        return token;
    }
}

SchemeExpr parse(const std::string& program)
{
    auto tokens = tokenize(program);
    return readFromTokens(tokens);
}
