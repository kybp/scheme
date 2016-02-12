#ifndef SCHEME_HH
#define SCHEME_HH

#include <deque>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include <boost/variant.hpp>

struct SchemeFunction;

enum class Nil { Nil };

struct SchemeSymbol {
    std::string string;

    bool operator==(const SchemeSymbol& rhs) const {
        return rhs.string == string;
    }

    bool operator!=(const SchemeSymbol& rhs) const {
        return !(*this == rhs);
    }
};

typedef boost::make_recursive_variant<
    int, char, bool, std::string, SchemeSymbol, Nil,
    std::shared_ptr<SchemeFunction>,
    std::pair<boost::recursive_variant_, boost::recursive_variant_>
    >::type SchemeExpr;
std::ostream& operator<<(std::ostream& os, const SchemeExpr& e);

typedef std::pair<SchemeExpr, SchemeExpr> SchemeCons;

class scheme_error : public std::exception {
    const std::string what_;
public:
    scheme_error(const std::string& what) : what_(what) {}
    scheme_error(const std::ostringstream& s) : what_(s.str()) {}
    virtual const char *what() const noexcept override
        { return what_.c_str(); }
};

SchemeCons consFromVector(const std::vector<SchemeExpr>& vector);
std::vector<SchemeExpr> vectorFromCons(SchemeCons cons);

class SchemeEnvironment
    : public std::enable_shared_from_this<SchemeEnvironment> {
    std::map<std::string, SchemeExpr> definitions;
    std::shared_ptr<SchemeEnvironment> outer;
public:
    SchemeEnvironment() = default;

    SchemeEnvironment(const std::vector<SchemeSymbol>& params,
                      const std::vector<SchemeExpr>& args)
        : SchemeEnvironment(params, args, false)
    {}

    SchemeEnvironment(const std::vector<SchemeSymbol>& params,
                      const std::vector<SchemeExpr>& args,
                      bool hasRestParam)
        : SchemeEnvironment(params, args, hasRestParam, nullptr)
    {}

    SchemeEnvironment(const std::vector<SchemeSymbol>& params,
                      const std::vector<SchemeExpr>& args,
                      bool hasRestParam,
                      std::shared_ptr<SchemeEnvironment> outer);
      
    std::shared_ptr<SchemeEnvironment> find(const std::string& var) {
        if (definitions.find(var) != definitions.end()) {
            return shared_from_this();
        } else if (outer != nullptr) {
            return outer->find(var);
        } else {
            return nullptr;
        }
    }

    SchemeExpr& operator[](const std::string& symbol) {
        return definitions[symbol];
    }
};

inline SchemeCons consValue(const SchemeExpr& e)
{
    try {
        return boost::get<SchemeCons>(e);
    } catch (const boost::bad_get&) {
        std::ostringstream error;
        error << "Cons expected, got " << e;
        throw scheme_error(error);
    }
}

inline SchemeExpr car(const SchemeCons& c)
{
    return c.first;
}

inline SchemeExpr cdr(const SchemeCons& c)
{
    return c.second;
}

inline std::vector<SchemeExpr> vectorFromExpr(const SchemeExpr& expr)
{
    if (SchemeExpr(Nil::Nil) == expr) {
        return {};
    } else {
        return vectorFromCons(consValue(expr));
    }
}

#endif
