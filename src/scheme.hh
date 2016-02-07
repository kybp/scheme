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
    virtual const char *what() const noexcept override
        { return what_.c_str(); }
};

inline int intValue(const SchemeExpr& e)
{
    try {
        return boost::get<int>(e);
    } catch (const boost::bad_get&) {
        std::ostringstream error;
        error << "Integer expected, got " << e;
        throw scheme_error(error.str());
    }
}

inline std::string stringValue(const SchemeExpr& e)
{
    try {
        return boost::get<std::string>(e);
    } catch (const boost::bad_get&) {
        std::ostringstream error;
        error << "Symbol expected, got " << e;
        throw scheme_error(error.str());
    }
}

inline bool boolValue(const SchemeExpr& e)
{
    try {
        return boost::get<bool>(e);
    } catch (const boost::bad_get&) {
        std::ostringstream error;
        error << "Boolean expected, got " << e;
        throw scheme_error(error.str());
    }
}

inline std::shared_ptr<SchemeFunction> functionPointer(const SchemeExpr& e)
{
    try {
        return boost::get<std::shared_ptr<SchemeFunction>>(e);
    } catch (const boost::bad_get&) {
        std::ostringstream error;
        error << "Function expected, got " << e;
        throw scheme_error(error.str());
    }
}

inline SchemeList listValue(const SchemeExpr& e)
{
    try {
        return boost::get<std::deque<SchemeExpr>>(e);
    } catch (const boost::bad_get&) {
        std::ostringstream error;
        error << "List expected, got " << e;
        throw scheme_error(error.str());
    }
}

typedef std::map<std::string, SchemeExpr> SchemeEnvironment;

struct SchemeFunction {
    std::function<SchemeExpr(std::vector<SchemeExpr>)> fn;
    SchemeEnvironment env;
    SchemeFunction(std::function<SchemeExpr(std::vector<SchemeExpr>)> fn)
        : fn(fn), env()
        {}
};

std::deque<std::string> tokenize(const std::string string);
SchemeExpr parse(const std::string& program);
SchemeExpr eval(SchemeExpr e, SchemeEnvironment& env);
bool isInteger(const std::string token);

#endif
