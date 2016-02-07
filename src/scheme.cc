#include <algorithm>
#include <cctype>  // for isdigit
#include <cstdlib> // for abs
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
    auto begin = token.begin();
    auto end   = token.end();
    if (begin != end && *begin == '-') ++begin;
    return std::all_of(begin, end, ::isdigit);
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

SchemeExpr scmAbs(std::vector<SchemeExpr> args)
{
    return { std::abs(intValue(args.front())) };
}

SchemeExpr scmAdd(std::vector<SchemeExpr> args)
{
    std::vector<int> ints;
    std::transform(args.begin(), args.end(), back_inserter(ints), intValue);
    return { std::accumulate(ints.begin(), ints.end(), 0) };
}

SchemeEnvironment standardEnvironment()
{
    SchemeEnvironment env;

    env["+"] = SchemeExpr(std::make_shared<SchemeFunction>(scmAdd));
    env["abs"] = SchemeExpr(std::make_shared<SchemeFunction>(scmAbs));

    return env;
}

class evalVisitor : public boost::static_visitor<SchemeExpr> {
    mutable SchemeEnvironment env;
public:
    evalVisitor(SchemeEnvironment env = standardEnvironment()) : env(env) {}

    SchemeExpr operator()(int i) const {
        return { i };
    }

    SchemeExpr operator()(const std::string& symbol) const {
        return { env[symbol] };
    }

    SchemeExpr operator()(const std::shared_ptr<SchemeFunction>& fn) const {
        return { fn };
    }

    SchemeExpr operator()(const std::deque<SchemeExpr>& list) const {
        const auto& car = boost::get<std::string>(list[0]);
        if (car == "quote") {
            return { list[1] };
        }
        else if (car == "if") {
            (void)boost::apply_visitor(evalVisitor(env), list[1]);
            if (true) { // how do I test a SchemeExpr ?
                return list[2];
            } else {
                return list[3];
            }
        } else if (car == "define") {
            auto var = boost::get<std::string>(list[1]);
            env[var] = boost::apply_visitor(evalVisitor(env), list[2]);
            return list[1];
        } else {
            auto car = boost::apply_visitor(evalVisitor(env), list[0]);
            auto proc = boost::get<std::shared_ptr<SchemeFunction>>(car);
            auto evalVisitorArg = [this](SchemeExpr e)
                { return boost::apply_visitor(evalVisitor(env), e); };
            std::vector<SchemeExpr> args;
            std::transform(list.begin() + 1, list.end(),
                           back_inserter(args), evalVisitorArg );
            return proc->fn(args);
        }
    }
};

SchemeExpr eval(SchemeExpr e)
{
    return boost::apply_visitor(evalVisitor(standardEnvironment()), e);
}

SchemeExpr eval(SchemeExpr e, SchemeEnvironment env = standardEnvironment())
{
    return boost::apply_visitor(evalVisitor(env), e);
}
