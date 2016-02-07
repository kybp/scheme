#ifndef SCHEME_HH
#define SCHEME_HH

#include <deque>
#include <iostream>
#include <map>
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

inline int intValue(const SchemeExpr& e)
{
    return boost::get<int>(e);
}

inline std::string stringValue(const SchemeExpr& e)
{
    return boost::get<std::string>(e);
}

inline bool boolValue(const SchemeExpr& e)
{
    return boost::get<bool>(e);
}

inline std::shared_ptr<SchemeFunction> functionPointer(const SchemeExpr& e)
{
    return boost::get<std::shared_ptr<SchemeFunction>>(e);
}

inline SchemeList listValue(const SchemeExpr& e)
{
    return boost::get<std::deque<SchemeExpr>>(e);
}

typedef std::map<std::string, SchemeExpr> SchemeEnvironment;
SchemeEnvironment standardEnvironment();

struct SchemeFunction {
    std::function<SchemeExpr(std::vector<SchemeExpr>)> fn;
    SchemeEnvironment env;
    SchemeFunction(std::function<SchemeExpr(std::vector<SchemeExpr>)> fn)
        : fn(fn), env()
        {}
};

std::deque<std::string> tokenize(const std::string string);
SchemeExpr parse(const std::string& program);
SchemeExpr eval(SchemeExpr e);
SchemeExpr eval(SchemeExpr e, SchemeEnvironment& env);

#endif
