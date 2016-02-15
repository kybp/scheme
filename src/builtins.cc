#include <algorithm>
#include <cstdlib>   // for std::abs
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
    std::vector<int> ints(args.size());
    std::transform(args.begin(), args.end(), ints.begin(), intValue);
    return std::accumulate(ints.begin(), ints.end(), 0);
}

SchemeExpr sub(const SchemeArgs& args)
{
    if (args.empty()) {
        throw scheme_error("- requires at least one argument, passed 0");
    } else if (args.size() == 1) {
        return -intValue(args.front());
    } else {
        std::vector<int> ints(args.size());
        std::transform(args.begin(), args.end(), ints.begin(), intValue);
        return std::accumulate(ints.begin() + 1, ints.end(), ints.front(),
                               std::minus<int>());
    }
}

SchemeExpr mul(const SchemeArgs& args)
{
    std::vector<int> ints(args.size());
    std::transform(args.begin(), args.end(), ints.begin(), intValue);
    return std::accumulate(ints.begin(), ints.end(), 1,
                           std::multiplies<int>());
}

SchemeExpr car(const SchemeArgs& args)
{
    if (args.size() != 1) {
        std::ostringstream error;
        error << "car requires one argument, passed " << args.size();
        throw scheme_error(error);
    } else {
        return ::car(consValue(args.front()));
    }
}

SchemeExpr cdr(const SchemeArgs& args)
{
    if (args.size() != 1) {
        std::ostringstream error;
        error << "cdr requires one argument, passed " << args.size();
        throw scheme_error(error);
    } else {
        return ::cdr(consValue(args.front()));
    }
}

SchemeExpr cons(const SchemeArgs& args) {
    if (args.size() != 2) {
        std::ostringstream error;
        error << "cons requires two arguments, passed " << args.size();
        throw scheme_error(error);
    } else {
        return SchemeCons{ args[0], args[1] };
    }
}

SchemeExpr nullp(const SchemeArgs& args)
{
    if (args.size() != 1) {
        std::ostringstream error;
        error << "null? requires one argument, passed " << args.size();
        throw scheme_error(error);
    } else {
        return SchemeExpr(Nil::Nil) == args.front();
    }
}

SchemeExpr length(const SchemeArgs& args)
{
    if (args.size() != 1) {
        std::ostringstream error;
        error << "length requires one argument, passed " << args.size();
        throw scheme_error(error);
    } else {
        return static_cast<int>(vectorFromExpr(args.front()).size());
    }
}

SchemeExpr lesser(const SchemeArgs& args)
{
    if (args.size() < 2) {
        std::ostringstream error;
        error << "< requires at least two arguments, passed " << args.size();
        throw scheme_error(error);
    } else {
        std::vector<int> ints(args.size());
        std::transform(args.begin(), args.end(), ints.begin(), intValue);
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
        std::vector<int> ints(args.size());
        std::transform(args.begin(), args.end(), ints.begin(), intValue);
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
        std::vector<int> ints(args.size());
        std::transform(args.begin(), args.end(), ints.begin(), intValue);
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
        return symbolValue(args[0]) == symbolValue(args[1]);
    }
}

SchemeExpr equalp(const SchemeArgs& args)
{
    if (args.size() != 2) {
        std::ostringstream error;
        error << "equal? requires two arguments, passed " << args.size();
        throw scheme_error(error);
    } else {
        return args[0] == args[1];
    }
}

SchemeExpr stringLength(const SchemeArgs& args)
{
    if (args.size() != 1) {
        std::ostringstream error;
        error << "string-length requires one argument, passed " << args.size();
        throw scheme_error(error);
    } else {
        return static_cast<int>(stringValue(args.front()).length());
    }
}

SchemeExpr stringRef(const SchemeArgs& args)
{
    if (args.size() != 2) {
        std::ostringstream error;
        error << "string-ref requires two arguments, passed " << args.size();
        throw scheme_error(error);
    } else {
        auto string = stringValue(args[0]);
        auto index  = static_cast<std::size_t>(intValue(args[1]));
        if (index >= string.length()) {
            std::ostringstream error;
            error << "Index " << index << " out of bounds for " << string;
            throw scheme_error(error);
        } else {
            return string[index];
        }
    }
}

SchemeExpr display(const SchemeArgs& args)
{
    if (args.size() != 1) {
        std::ostringstream error;
        error << "display requires one argument, passed " << args.size();
        throw scheme_error(error);
    } else {
        try {
            std::cout << boost::get<std::string>(args.front());
        } catch (const boost::bad_get&) {
            std::cout << args.front();
        }
        return false;
    }
}

SchemeExpr newline(const SchemeArgs& args)
{
    if (!args.empty()) {
        std::ostringstream error;
        error << "newline does not take arguments, passed " << args.size();
        throw scheme_error(error);
    } else {
        std::cout << std::endl;
        return false;
    }
}

SchemeExpr characterp(const SchemeArgs& args)
{
    if (args.size() != 1) {
        std::ostringstream error;
        error << "character? requires one argument, passed " << args.size();
        throw scheme_error(error);
    } else {
        try {
            charValue(args.front());
            return true;
        } catch (const scheme_error&) {
            return false;
        }
    }
}

SchemeExpr consp(const SchemeArgs& args)
{
    if (args.size() != 1) {
        std::ostringstream error;
        error << "cons? requires one argument, passed " << args.size();
        throw scheme_error(error);
    } else {
        try {
            consValue(args.front());
            return true;
        } catch (const scheme_error&) {
            return false;
        }
    }
}

SchemeExpr numberp(const SchemeArgs& args)
{
    if (args.size() != 1) {
        std::ostringstream error;
        error << "number? requires one argument, passed " << args.size();
        throw scheme_error(error);
    } else {
        try {
            intValue(args.front());
            return true;
        } catch (const scheme_error&) {
            return false;
        }
    }
}

SchemeExpr stringp(const SchemeArgs& args)
{
    if (args.size() != 1) {
        std::ostringstream error;
        error << "string? requires one argument, passed " << args.size();
        throw scheme_error(error);
    } else {
        try {
            stringValue(args.front());
            return true;
        } catch (const scheme_error&) {
            return false;
        }
    }
}

SchemeExpr symbolp(const SchemeArgs& args)
{
    if (args.size() != 1) {
        std::ostringstream error;
        error << "symbol? requires one argument, passed " << args.size();
        throw scheme_error(error);
    } else {
        try {
            symbolValue(args.front());
            return true;
        } catch (const scheme_error&) {
            return false;
        }
    }
}

} // end namespace

SchemeExpr eval(const SchemeExpr& e)
{
    return boost::apply_visitor(evalVisitor(standardEnvironment()), e);
}

inline void addPrimitive(std::vector<SchemeSymbol>& names,
                         std::vector<SchemeExpr>& functions,
                         const std::string& name,
                         SchemeExpr (*function)(const SchemeArgs&))
{
    auto primitive = SchemeExpr(std::make_shared<PrimitiveFunction>(function));
    names.push_back(symbolValue(parse(name)));
    functions.push_back(primitive);
}

std::shared_ptr<SchemeEnvironment> standardEnvironment()
{
    std::vector<SchemeSymbol> names;
    std::vector<SchemeExpr> functions;

    addPrimitive(names, functions, "+", scheme::add);
    addPrimitive(names, functions, "-", scheme::sub);
    addPrimitive(names, functions, "*", scheme::mul);
    addPrimitive(names, functions, "<", scheme::lesser);
    addPrimitive(names, functions, ">", scheme::greater);
    addPrimitive(names, functions, "=", scheme::equal);
    addPrimitive(names, functions, "abs", scheme::abs);
    addPrimitive(names, functions, "car", scheme::car);
    addPrimitive(names, functions, "character?", scheme::characterp);
    addPrimitive(names, functions, "cdr", scheme::cdr);
    addPrimitive(names, functions, "cons", scheme::cons);
    addPrimitive(names, functions, "cons?", scheme::consp);
    addPrimitive(names, functions, "display", scheme::display);
    addPrimitive(names, functions, "eq?", scheme::eq);
    addPrimitive(names, functions, "equal?", scheme::equalp);
    addPrimitive(names, functions, "length", scheme::length);
    addPrimitive(names, functions, "newline", scheme::newline);
    addPrimitive(names, functions, "not", scheme::_not);
    addPrimitive(names, functions, "null?", scheme::nullp);
    addPrimitive(names, functions, "number?", scheme::numberp);
    addPrimitive(names, functions, "string?", scheme::stringp);
    addPrimitive(names, functions, "string-length", scheme::stringLength);
    addPrimitive(names, functions, "string-ref", scheme::stringRef);
    addPrimitive(names, functions, "symbol?", scheme::symbolp);

    return std::make_shared<SchemeEnvironment>(
        SchemeEnvironment(names, functions));
}
