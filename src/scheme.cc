#include <algorithm>
#include <cctype>
#include <deque>
#include <iterator>
#include <numeric>
#include <string>
#include <sstream>
#include <vector>
#include <boost/variant.hpp>
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
    return std::all_of(token.begin(), token.end(), ::isdigit);
}

SchemeExpr readFromTokens(std::deque<std::string>& tokens)
{
    if (tokens.empty()) {
        throw std::runtime_error("unexpected EOF while reading");
    }
    std::string token = tokens.front();
    tokens.pop_front();
    if (token == "(") {
        std::deque<SchemeExpr> list;
        while (tokens.front() != ")") {
            list.push_back(readFromTokens(tokens));
        }
        tokens.pop_front();     // remove ")"
        return { list };
    } else if (token == ")") {
        throw std::runtime_error("unexpected ')'");
    } else if (isInteger(token)) {
        return { std::atoi(token.c_str()) };
    } else {
        return { token };
    }
}

SchemeExpr parse(const std::string& program)
{
    auto tokens = tokenize(program);
    return readFromTokens(tokens);
}

SchemeExpr add(std::initializer_list<SchemeExpr> args)
{
    std::vector<int> ints;
    std::transform(args.begin(), args.end(), back_inserter(ints), intValue);
    return { std::accumulate(ints.begin(), ints.end(), 0) };
}

SchemeEnvironment standardEnvironment() {
    SchemeEnvironment env;

    env["+"] = SchemeExpr(std::make_shared<SchemeFunction>(add));

    return env;
}
