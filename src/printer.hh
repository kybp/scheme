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

    std::string operator()(bool b) const {
        os << (b ? "#t" : "#f");
        return os.str();
    }

    std::string operator()(char c) const {
        os << "#\\" << c;
        return os.str();
    }

    std::string operator()(int i) const {
        os << i;
        return os.str();
    }

    std::string operator()(const std::string& string) const {
        os << '"' << string << '"';
        return os.str();
    }

    std::string operator()(const SchemeSymbol& symbol) const {
        os << symbol.string;
        return os.str();
    }

    std::string operator()(const std::shared_ptr<SchemeFunction>&) const {
        os << "<function>";
        return os.str();
    }

    std::string operator()(const Nil&) const {
        os << "()";
        return os.str();
    }

    std::string operator()(SchemeCons cons) const {
        os << "(";
        for (;;) {
            os << car(cons);
            if (SchemeExpr(Nil::Nil) == cdr(cons)) {
                break;
            } else {
                os << " ";
            }
            try {
                cons = consValue(cdr(cons));
            } catch (const scheme_error&) {
                os << ". " << cdr(cons);
                break;
            }
        }
        os << ")";
        return os.str();
    }
};

inline std::ostream& operator<<(std::ostream& os, const SchemeExpr& e)
{
    // This and the list case in stringVisitor don't use symbolValue()
    // to avoid a circular dependency with parser.hh
    std::ostringstream ss;
    boost::apply_visitor(stringVisitor(ss), e);
    return os << ss.str();
}

#endif
