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
    in >> std::noskipws;
    std::ostringstream token;
    char c;

    while (in >> c) {
        if (c == '(' || c == ')' || c == '"') {
            if (token.str().empty()) token << c;
            else in.putback(c);
            break;
        } else if (std::isspace(c)) {
            if (!token.str().empty()) break;
            // else continue reading
        } else {
            token << c;
        }
    }

    out = token.str();
    if (in.eof() && !out.empty()) {
        in.putback(EOF);
        in.clear();
    }
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

std::istream& readUntil(std::istream& in, char delimiter, std::string& out)
{
    in >> std::noskipws;
    std::ostringstream string;
    char c;
    while (in >> c && c != delimiter) string << c;
    out = string.str();
    return in;
}

std::istream& readSchemeExpr(std::istream& in, SchemeExpr& out)
{
    std::string token;
    if (!readToken(in, token)) return in;

    if (token == "(") {
        std::vector<SchemeExpr> listVec;
        for (;;) {
            std::string element;
            if (!readToken(in, element)) {
                throw scheme_error("Unmatched '('");
            }
            if (element == ")") break;
            SchemeExpr expr;
            if (element == "(") {
                in.putback('(');
                readSchemeExpr(in, expr);
            } else {
                readSchemeExpr(element, expr);
            }
            listVec.push_back(expr);
        }

        out = listVec.empty() ? SchemeExpr(Nil::Nil) : consFromVector(listVec);
    } else if (token == ")") {
        throw scheme_error("Unexpected ')'");
    } else if (isInteger(token)) {
        out = std::atoi(token.c_str());
    } else if (token == "#t") {
        out = true;
    } else if (token == "#f") {
        out = false;
    } else if (token == "\"") {
        std::string string;
        if (readUntil(in, '"', string)) out = string;
        else throw scheme_error("Unclosed string literal");
    } else {
        out = SchemeSymbol{token};
    }

    return in;
}

// parse() is only retained for testing

SchemeExpr parse(const std::string& program)
{
    std::istringstream in(program);
    SchemeExpr expr;
    readSchemeExpr(in, expr);
    return expr;
}
