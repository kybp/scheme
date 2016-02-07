#ifndef SCHEME_HH
#define SCHEME_HH

#include <deque>
#include <map>
#include <string>
#include <boost/optional.hpp>
#include <boost/variant.hpp>

struct SchemeFunction;

typedef boost::make_recursive_variant<
    int, std::shared_ptr<SchemeFunction>, std::string,
    std::deque<boost::recursive_variant_>
    >::type SchemeExpr;

inline int intValue(SchemeExpr e)
{
    return boost::get<int>(e);
}

typedef std::map<std::string, SchemeExpr> SchemeEnvironment;

struct SchemeFunction {
    std::function<SchemeExpr(std::initializer_list<SchemeExpr>)> fn;
    boost::optional<int> numArgs;
    SchemeEnvironment env;
    SchemeFunction(std::function<
                   SchemeExpr(std::initializer_list<SchemeExpr>)> fn)
        : fn(fn), env()
        {}
    // SchemeFunction(std::function<
    //                SchemeExpr(std::initializer_list<SchemeExpr>)> fn,
    //                SchemeEnvironment env)
    //     : fn(fn), env(env)
    //     {}
    // SchemeFunction(std::function<
    //                SchemeExpr(std::initializer_list<SchemeExpr>)> fn,
    //                int numArgs, SchemeEnvironment env)
    //     : fn(fn), numArgs(numArgs), env(env)
    //     {}
};

std::deque<std::string> tokenize(const std::string string);
SchemeExpr parse(const std::string& program);

#endif
