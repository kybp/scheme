#include <algorithm> // for std::transform, std::all_of
#include <cctype>    // for std::isdigit
#include <deque>
#include <string>
#include <sstream>
#include <vector>
#include <boost/variant.hpp>
#include "parser.hh"
#include "scheme_types.hh"

std::istream& readToken(std::istream& in, std::string& out)
{
    std::ostringstream token;
    char c;

    while (in >> c) {
        if (c == '(' || c == ')') {
            if (token.str().empty()) token << c;
            else in.putback(c);
            break;
        } else if (std::isspace(c)) {
            if (!token.str().empty()) break;
            // else continue reading
        } else if (c == EOF) {
            break;
        } else {
            token << c;
        }
    }

    out = token.str();
    return in;
}

bool isInteger(const std::string token)
{
    auto begin = token.begin();
    auto end   = token.end();
    if (begin != end && *begin == '-') ++begin;
    if (begin == end) return false;
    return std::all_of(begin, end, ::isdigit);
}

std::istream& readSchemeExpr(std::istream& in, SchemeExpr& out);

void readSchemeExpr(const std::string& string, SchemeExpr& out)
{
    std::istringstream in(string);
    readSchemeExpr(in, out);
}

std::istream& readSchemeExpr(std::istream& in, SchemeExpr& out)
{
    std::string token;
    readToken(in, token);
    if (!in && token.empty()) {
        throw scheme_error("Unexpected EOF while reading");
    }

    if (token == "(") {
        SchemeList list;
        std::string element;
        while (readToken(in, element)) {
            if (element == ")") break;
            SchemeExpr expr;
            if (element == "(") {
                in.putback('(');
                readSchemeExpr(in, expr);
            } else {
                readSchemeExpr(element, expr);
            }
            list.push_back(expr);
        }
        out = list;
    } else if (token == ")") {
        throw scheme_error("Unexpected ')");
    } else if (isInteger(token)) {
        out = std::atoi(token.c_str());
    } else if (token == "#t") {
        out = true;
    } else if (token == "#f") {
        out = false;
    } else {
        out = token;
    }

    return in;
}

// tokenize() and parse() are only retained for testing

std::deque<std::string> tokenize(const std::string string)
{
    std::deque<std::string> tokens;
    std::string token;
    std::istringstream in(string);
    in >> std::noskipws;

    while (readToken(in, token)) {
        tokens.push_back(token);
    }

    if (!token.empty()) tokens.push_back(token);

    return tokens;
}

SchemeExpr parse(const std::string& program)
{
    std::istringstream in(program);
    in >> std::noskipws;
    SchemeExpr expr;
    readSchemeExpr(in, expr);
    return expr;
}
