#include <algorithm>
#include <cstdlib>   // for std::abs
#include <iterator>  // for std::back_inserter
#include <numeric>   // for std::accumulate
#include <boost/variant.hpp>
#include "builtins.hh"
#include "eval.hh"
#include "scheme_types.hh"

namespace scheme {

SchemeExpr abs(const SchemeArgs& args)
{
    return std::abs(intValue(args.front()));
}

SchemeExpr add(const SchemeArgs& args)
{
    std::vector<int> ints;
    std::transform(args.begin(), args.end(), back_inserter(ints), intValue);
    return std::accumulate(ints.begin(), ints.end(), 0);
}

SchemeExpr sub(const SchemeArgs& args)
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

SchemeExpr mul(const SchemeArgs& args)
{
    auto mul = [](int x, int y) { return x * y; };
    std::vector<int> ints;
    std::transform(args.begin(), args.end(), back_inserter(ints), intValue);
    return std::accumulate(ints.begin(), ints.end(), 1, mul);
}

SchemeExpr lesser(const SchemeArgs& args)
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

SchemeExpr equal(const SchemeArgs& args)
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

SchemeExpr greater(const SchemeArgs& args)
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

SchemeExpr _not(const SchemeArgs& args)
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

SchemeExpr eq(const SchemeArgs& args)
{
    if (args.size() != 2) {
        std::ostringstream error;
        error << "eq? requires two arguments, passed " << args.size();
        throw scheme_error(error);
    } else {
        return stringValue(args[0]) == stringValue(args[1]);
    }
}

} // end namespace

SchemeExpr eval(const SchemeExpr& e)
{
    return boost::apply_visitor(evalVisitor(standardEnvironment()), e);
}

inline void addPrimitive(std::vector<std::string>& names,
                         std::vector<SchemeExpr>& functions,
                         const std::string& name,
                         SchemeExpr (*function)(const SchemeArgs&))
{
    auto primitive = SchemeExpr(std::make_shared<PrimitiveFunction>(function));
    names.push_back(name);
    functions.push_back(primitive);
}

std::shared_ptr<SchemeEnvironment> standardEnvironment()
{
    std::vector<std::string> names;
    std::vector<SchemeExpr> functions;

    addPrimitive(names, functions, "+", scheme::add);
    addPrimitive(names, functions, "-", scheme::sub);
    addPrimitive(names, functions, "*", scheme::mul);
    addPrimitive(names, functions, "<", scheme::lesser);
    addPrimitive(names, functions, ">", scheme::greater);
    addPrimitive(names, functions, "=", scheme::equal);
    addPrimitive(names, functions, "abs", scheme::abs);
    addPrimitive(names, functions, "eq?", scheme::eq);
    addPrimitive(names, functions, "not", scheme::_not);

    return std::make_shared<SchemeEnvironment>(
        SchemeEnvironment(names, functions));
}
