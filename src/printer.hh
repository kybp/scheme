#ifndef PRINTER_HH
#define PRINTER_HH

#include <memory>
#include <sstream>
#include <string>
#include <boost/variant.hpp>
#include "scheme_types.hh"

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

    std::string operator()(const SchemeList& list) const {
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

inline std::ostream& operator<<(std::ostream& os, const SchemeExpr& e)
{
    std::ostringstream ss;
    boost::apply_visitor(stringVisitor(ss), e);
    return os << ss.str();
}

#endif
