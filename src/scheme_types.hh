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

typedef boost::make_recursive_variant<
    int, std::shared_ptr<SchemeFunction>, std::string, bool,
    std::pair<boost::recursive_variant_, boost::recursive_variant_>, Nil
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

class SchemeEnvironment
    : public std::enable_shared_from_this<SchemeEnvironment> {
    std::map<std::string, SchemeExpr> definitions;
    std::shared_ptr<SchemeEnvironment> outer;
public:
    SchemeEnvironment() = default;

    SchemeEnvironment(const std::vector<std::string>& params,
                      const std::vector<SchemeExpr>& args)
        : SchemeEnvironment(params, args, nullptr)
    {}

    SchemeEnvironment(const std::vector<std::string>& params,
                      const std::vector<SchemeExpr>& args,
                      std::shared_ptr<SchemeEnvironment> outer)
        : outer(outer)
    {
        for (std::size_t i = 0; i < args.size(); ++i) {
            definitions[params[i]] = args[i];
        }
    }

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

inline std::vector<SchemeExpr> vectorFromCons(SchemeCons cons)
{
    std::vector<SchemeExpr> v;

    for (;;) {
        v.push_back(car(cons));
        if (SchemeExpr(Nil::Nil) == cdr(cons)) break;
        cons = consValue(cdr(cons));
    }

    return v;
}

inline std::vector<SchemeExpr> vectorFromExpr(const SchemeExpr& expr)
{
    if (SchemeExpr(Nil::Nil) == expr) {
        return {};
    } else {
        return vectorFromCons(consValue(expr));
    }
}

inline SchemeCons consFromVector(const std::vector<SchemeExpr>& vector) {
    if (vector.empty()) {
        throw std::runtime_error("Can't convert empty vector to cons");
    }

    SchemeCons cons{ vector.back(), SchemeExpr(Nil::Nil) };
    SchemeCons prev;
    for (auto it = vector.rbegin() + 1; it != vector.rend(); ++it) {
        prev = { *it, cons };
        cons = prev;
    }

    return cons;
}

#endif
