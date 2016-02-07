#include <cstdlib>   // for abs
#include <numeric>   // for std::accumulate
#include <boost/variant.hpp>
#include "builtins.hh"
#include "eval.hh"
#include "scheme.hh"

SchemeExpr eval(SchemeExpr e)
{
    SchemeEnvironment env = standardEnvironment();
    return boost::apply_visitor(evalVisitor(env), e);
}

SchemeExpr scmAbs(std::vector<SchemeExpr> args)
{
    return { std::abs(intValue(args.front())) };
}

SchemeExpr scmAdd(std::vector<SchemeExpr> args)
{
    std::vector<int> ints;
    std::transform(args.begin(), args.end(), back_inserter(ints), intValue);
    return { std::accumulate(ints.begin(), ints.end(), 0) };
}

SchemeEnvironment standardEnvironment()
{
    SchemeEnvironment env;

    env["+"] = SchemeExpr(std::make_shared<SchemeFunction>(scmAdd));
    env["abs"] = SchemeExpr(std::make_shared<SchemeFunction>(scmAbs));

    return env;
}

