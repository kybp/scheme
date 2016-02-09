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

typedef boost::make_recursive_variant<
    int, std::shared_ptr<SchemeFunction>, std::string, bool,
    std::deque<boost::recursive_variant_>
    >::type SchemeExpr;
std::ostream& operator<<(std::ostream& os, const SchemeExpr& e);

typedef std::deque<SchemeExpr> SchemeList;

class scheme_error : public std::exception {
    const std::string what_;
public:
    scheme_error(const std::string& what) : what_(what) {}
    scheme_error(const std::ostringstream& s) : what_(s.str()) {}
    virtual const char *what() const noexcept override
        { return what_.c_str(); }
};

class SchemeEnvironment {
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

    SchemeEnvironment *find(const std::string& var) {
        if (definitions.find(var) != definitions.end()) {
            return this;
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

#endif
