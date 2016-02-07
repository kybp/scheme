#include <algorithm> // for std::transform, std::all_of
#include <cctype>    // for std::isdigit
#include <deque>
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
    } else if (token == "#t") {
        return { true };
    } else if (token == "#f") {
        return { false };
    } else {
        return { token };
    }
}

SchemeExpr parse(const std::string& program)
{
    auto tokens = tokenize(program);
    return readFromTokens(tokens);
}

class stringVisitor : public boost::static_visitor<SchemeExpr> {
    std::ostringstream& os;
public:
    stringVisitor(std::ostringstream& os) : os(os) {}

    std::string operator()(int i) const {
        os << i;
        return os.str();
    }

    std::string operator()(bool b) const {
        os << (b ? "#t" : "#f");
        return os.str();
    }

    std::string operator()(const std::string& symbol) const {
        os << symbol;
        return symbol;
    }

    std::string operator()(const std::shared_ptr<SchemeFunction>&) const {
        os << "<function>";
        return os.str();
    }

    std::string operator()(const std::deque<SchemeExpr>& list) const {
        os << "(";
        switch (list.size()) {
        case 0:
            break;
        case 1:
            boost::apply_visitor(stringVisitor(os), list[0]);
            break;
        default:
            os << list.front();
            for (auto it = list.cbegin() + 1; it != list.cend(); ++it) {
                os << " ";
                boost::apply_visitor(stringVisitor(os), *it);
            }
        }
        os << ")";
        return os.str();
    }
};

std::ostream& operator<<(std::ostream& os, const SchemeExpr& e)
{
    std::ostringstream ss;
    boost::apply_visitor(stringVisitor(ss), e);
    return os << ss.str();
}
