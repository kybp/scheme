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
    return boost::apply_visitor(evalVisitor(standardEnvironment()), e);
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

SchemeExpr scmLesser(const SchemeArgs& args)
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

SchemeExpr scmGreater(const SchemeArgs& args)
{
    if (args.size() < 2) {
        std::ostringstream error;
        error << "> requires at least two arguments, passed " << args.size();
        throw scheme_error(error);
    } else {
        std::vector<int> ints;
        std::transform(args.begin(), args.end(), back_inserter(ints),
                       intValue);
        return std::adjacent_find(ints.begin(), ints.end(),
                                  std::less_equal<int>()) == ints.end();
    }
}

SchemeExpr scmNot(const SchemeArgs& args)
{
    if (args.size() != 1) {
        std::ostringstream error;
        error << "not requires one argument, passed " << args.size();
        throw scheme_error(error);
    } else {
        if (boolValue(args.front())) {
            return false;
        } else {
            return true;
        }
    }
}

SchemeExpr primitiveFunction(SchemeExpr (*fn)(const SchemeArgs&))
{
    return SchemeExpr(std::make_shared<PrimitiveFunction>(fn));
}

std::shared_ptr<SchemeEnvironment> standardEnvironment()
{
    std::vector<std::string> names;
    std::vector<SchemeExpr> functions;

    names.push_back("+"); functions.push_back(primitiveFunction(scmAdd));
    names.push_back("+"); functions.push_back(primitiveFunction(scmAdd));
    names.push_back("-"); functions.push_back(primitiveFunction(scmSub));
    names.push_back("*"); functions.push_back(primitiveFunction(scmMul));
    names.push_back("<"); functions.push_back(primitiveFunction(scmLesser));
    names.push_back(">"); functions.push_back(primitiveFunction(scmGreater));
    names.push_back("="); functions.push_back(primitiveFunction(scmEqual));
    names.push_back("abs"); functions.push_back(primitiveFunction(scmAbs));
    names.push_back("not"); functions.push_back(primitiveFunction(scmNot));

    return std::make_shared<SchemeEnvironment>(
        SchemeEnvironment(names, functions));
}
