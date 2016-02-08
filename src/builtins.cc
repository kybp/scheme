#include <algorithm> // for std::transform
#include <cstdlib>   // for std::abs
#include <iterator>  // for std::back_inserter
#include <numeric>   // for std::accumulate
#include <boost/variant.hpp>
#include "builtins.hh"
#include "eval.hh"
#include "scheme_types.hh"

SchemeExpr eval(SchemeExpr e)
{
    SchemeEnvironment env = standardEnvironment();
    return boost::apply_visitor(evalVisitor(env), e);
}

SchemeExpr scmAbs(SchemeArgs args)
{
    return std::abs(intValue(args.front()));
}

SchemeExpr scmAdd(SchemeArgs args)
{
    std::vector<int> ints;
    std::transform(args.begin(), args.end(), back_inserter(ints), intValue);
    return std::accumulate(ints.begin(), ints.end(), 0);
}

SchemeExpr scmSub(SchemeArgs args)
{
    if (args.empty()) {
        throw scheme_error("- requires at least one argument, passed 0");
    } else if (args.size() == 1) {
        return -intValue(args.front());
    } else {
        auto minus = [](int x, int y) { return x - y; };
        std::vector<int> ints;
        std::transform(args.begin(), args.end(), back_inserter(ints),
                       intValue);
        return std::accumulate(ints.begin() + 1, ints.end(),
                               ints.front(), minus);
    }
}

SchemeExpr scmMul(SchemeArgs args)
{
    auto mul = [](int x, int y) { return x * y; };
    std::vector<int> ints;
    std::transform(args.begin(), args.end(), back_inserter(ints), intValue);
    return std::accumulate(ints.begin(), ints.end(), 1, mul);
}

SchemeEnvironment standardEnvironment()
{
    SchemeEnvironment env;

    env["+"] = SchemeExpr(std::make_shared<SchemeFunction>(scmAdd));
    env["-"] = SchemeExpr(std::make_shared<SchemeFunction>(scmSub));
    env["*"] = SchemeExpr(std::make_shared<SchemeFunction>(scmMul));
    env["abs"] = SchemeExpr(std::make_shared<SchemeFunction>(scmAbs));

    return env;
}

