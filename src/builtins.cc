#include <algorithm> // for std::transform
#include <cstdlib>   // for std::abs
#include <iterator>  // for std::back_inserter
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

SchemeExpr scmAbs(SchemeArgs args)
{
    return { std::abs(intValue(args.front())) };
}

SchemeExpr scmAdd(SchemeArgs args)
{
    std::vector<int> ints;
    std::transform(args.begin(), args.end(), back_inserter(ints), intValue);
    return { std::accumulate(ints.begin(), ints.end(), 0) };
}

SchemeExpr scmSub(SchemeArgs args)
{
    auto minus = [](int x, int y) { return x - y; };
    std::vector<int> ints;
    std::transform(args.begin(), args.end(), back_inserter(ints), intValue);
    return { std::accumulate(ints.begin(), ints.end(), 0, minus) };
}

SchemeEnvironment standardEnvironment()
{
    SchemeEnvironment env;

    env["+"] = SchemeExpr(std::make_shared<SchemeFunction>(scmAdd));
    env["-"] = SchemeExpr(std::make_shared<SchemeFunction>(scmSub));
    env["abs"] = SchemeExpr(std::make_shared<SchemeFunction>(scmAbs));

    return env;
}

