#include <algorithm> // for std::transform, std::all_of
#include <cctype>    // for std::isdigit
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
        if (c == '(' || c == ')' || c == '"' || c == '\'' || c == '`') {
            if (token.str().empty()) token << c;
            else in.putback(c);
            break;
        } else if (c == ',') {
            if (token.str().empty()) {
                token << ",";
                if (in.peek() == '@') {
                    in >> c;
                    token << c;
                }
            } else in.putback(c);
            break;
        } else if (c == '#' && in.peek() == '\\') {
            if (token.str().empty()) {
                in >> c;        // read the backslash too
                token << "#\\";
            } else in.putback(c);
            break;
        } else if (c == ';') {
            if (token.str().empty()) {
                while (in >> c && c != '\n')
                    ;
            } else {
                in.putback(c);
                break;
            }
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

char designatorToChar(const std::string& designator)
{
    if (designator.empty()) throw scheme_error("char from empty designator");
    if (designator.length() == 1) return designator[0];

    std::string lowercase(designator);
    std::transform(designator.begin(), designator.end(), lowercase.begin(),
                   ::tolower);

         if (lowercase == "space")   return ' ';
    else if (lowercase == "newline") return '\n';
    else if (lowercase == "tab")     return '\t';
    else {
        std::ostringstream error;
        error << "Unrecognized character name: " << designator;
        throw scheme_error(error);
    }
}

std::istream& readChar(std::istream& in, char& out)
{
    in >> std::noskipws;
    std::ostringstream designator;
    char c;

    while (in >> c) {
        if (c == '(' || c == ')' || c == '"' || std::isspace(c)) {
            if (designator.str().empty()) designator << c;
            else in.putback(c);
            break;
        } else {
            designator << c;
        }
    }

    out = designatorToChar(designator.str());

    if (in.eof() && !designator.str().empty()) {
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

std::istream& readString(std::istream& in, std::string& out)
{
    in >> std::noskipws;
    std::ostringstream string;
    char c;
    bool escaped = false;
    while (in >> c) {
        if (escaped) {
            escaped = false;
            switch (c) {
            case 'n': string << '\n'; break;
            case 't': string << '\t'; break;
            default: string << c; break;
            }
        }
        else if (c == '\\') escaped = true;
        else if (c == '"') break;
        else string << c;
    }
    out = string.str();
    return in;
}

std::istream& readMacroQuoteForm(std::istream& in, const std::string& token,
                                 SchemeExpr& out)
{
    SchemeExpr expr;
    if (readSchemeExpr(in, expr)) {
        SchemeExpr sym;
        if (token == "'")       sym = SchemeSymbol("quote");
        else if (token == "`")  sym = SchemeSymbol("quasiquote");
        else if (token == ",")  sym = SchemeSymbol("unquote");
        else if (token == ",@") sym = SchemeSymbol("unquote-splicing");
        out = consFromVector(std::vector<SchemeExpr>{ sym, expr });
    }
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
            if (element == "(" || element == "\"") {
                in.putback(element[0]);
                readSchemeExpr(in, expr);
            } else if (element == "'" || element == "`" ||
                       element == "," || element == ",@") {
                if (!readMacroQuoteForm(in, element, expr))
                    throw scheme_error("Unexpected EOF in (quasi)quote");
            } else if (element == "#\\") {
                char c;
                if (readChar(in, c)) expr = c;
                else throw scheme_error("Unexpected EOF in character literal");
            } else {
                readSchemeExpr(element, expr);
            }
            listVec.push_back(expr);
        }

        out = listVec.empty() ? SchemeExpr(Nil::Nil) : consFromVector(listVec);
    } else if (token == "'" || token == "`" || token == "," || token == ",@") {
        if (!readMacroQuoteForm(in, token, out))
            throw scheme_error("Unexpected EOF in (quasi)quote");
    } else if (token == ")") {
        throw scheme_error("Unexpected ')'");
    } else if (token == "#\\") {
        char c;
        if (readChar(in, c)) out = c;
        else throw scheme_error("Unexpected EOF in character literal");
    } else if (isInteger(token)) {
        out = std::atoi(token.c_str());
    } else if (token == "#t") {
        out = true;
    } else if (token == "#f") {
        out = false;
    } else if (token == "\"") {
        std::string string;
        if (readString(in, string)) out = string;
        else throw scheme_error("Unclosed string literal");
    } else {
        out = SchemeSymbol{token};
    }

    return in;
}

SchemeExpr parse(const std::string& program)
{
    std::istringstream in(program);
    SchemeExpr expr;
    readSchemeExpr(in, expr);
    return expr;
}
