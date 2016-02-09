#include <algorithm> // for std::transform, std::is_sorted
#include <cstdlib>   // for std::abs
#include <iterator>  // for std::back_inserter
#include <numeric>   // for std::accumulate
#include <boost/variant.hpp>
#include "builtins.hh"
#include "eval.hh"
#include "scheme_types.hh"

SchemeExpr eval(const SchemeExpr& e)
{
    SchemeEnvironment env = standardEnvironment();
    return boost::apply_visitor(evalVisitor(env), e);
}

SchemeExpr scmAbs(const SchemeArgs& args)
{
    return std::abs(intValue(args.front()));
}

SchemeExpr scmAdd(const SchemeArgs& args)
{
    std::vector<int> ints;
    std::transform(args.begin(), args.end(), back_inserter(ints), intValue);
    return std::accumulate(ints.begin(), ints.end(), 0);
}

SchemeExpr scmSub(const SchemeArgs& args)
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

SchemeExpr scmMul(const SchemeArgs& args)
{
    auto mul = [](int x, int y) { return x * y; };
    std::vector<int> ints;
    std::transform(args.begin(), args.end(), back_inserter(ints), intValue);
    return std::accumulate(ints.begin(), ints.end(), 1, mul);
}

SchemeExpr scmLessThan(const SchemeArgs& args)
{
    if (args.size() < 2) {
        std::ostringstream error;
        error << "< requires at least two arguments, passed " << args.size();
        throw scheme_error(error);
    } else {
        std::vector<int> ints;
        std::transform(args.begin(), args.end(), back_inserter(ints),
                       intValue);
        return std::adjacent_find(ints.begin(), ints.end(),
                                  std::greater_equal<int>()) == ints.end();
    }
}

SchemeExpr scmEqual(const SchemeArgs& args)
{
    if (args.size() < 2) {
        std::ostringstream error;
        error << "= requires at least two arguments, passed " << args.size();
        throw scheme_error(error);
    } else {
        std::vector<int> ints;
        std::transform(args.begin(), args.end(), back_inserter(ints),
                       intValue);
        return std::adjacent_find(ints.begin(), ints.end(),
                                  std::not_equal_to<int>()) == ints.end();
    }
}

SchemeExpr primitiveFunction(SchemeExpr (*fn)(const SchemeArgs&))
{
    return SchemeExpr(std::make_shared<PrimitiveFunction>(fn));
}

SchemeEnvironment standardEnvironment()
{
    SchemeEnvironment env;

    env["+"] = primitiveFunction(scmAdd);
    env["-"] = primitiveFunction(scmSub);
    env["*"] = primitiveFunction(scmMul);
    env["<"] = primitiveFunction(scmLessThan);
    env["="] = primitiveFunction(scmEqual);
    env["abs"] = primitiveFunction(scmAbs);

    return env;
}

