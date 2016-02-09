#include <memory>
#include <sstream>
#include "eval.hh"
#include "parser.hh"
#include "scheme_types.hh"

SchemeExpr evalVisitor::evalAnd(const SchemeArgs& args, envPointer env) const
{
    SchemeExpr last = true;

    for (const SchemeExpr& e : args) {
        auto evalled = eval(e, env);
        try {
            if (!boolValue(evalled)) return false;
            else last = evalled;
        } catch (const scheme_error&) {
            last = evalled;
        }
    }

    return last;
}

SchemeExpr
evalVisitor::evalFuncall(const SchemeExpr& car, const SchemeArgs& args,
                         envPointer env) const
{
    auto evalVisitorArg = [env](SchemeExpr e) { return eval(e, env); };
    SchemeArgs evalledArgs;
    std::transform(args.begin(), args.end(),
                   back_inserter(evalledArgs), evalVisitorArg );
    return (*functionPointer(car))(evalledArgs);
}

SchemeExpr evalVisitor::evalIf(const SchemeArgs& args, envPointer env) const
{
    if (args.size() != 3) {
        std::ostringstream error;
        error << "if requires exactly 3 arguments, ";
        error << "passed " << args.size();
        throw scheme_error(error);
    }
    auto pred = eval(args[0], env);
    if (boolValue(pred)) {
        return eval(args[1], env);
    } else {
        return eval(args[2], env);
    }
}

SchemeExpr evalVisitor::evalLambda(const SchemeArgs& args, envPointer env) const
{
    auto procArgs = listValue(args[0]);
    std::vector<std::string> params;
    std::transform(procArgs.begin(), procArgs.end(), back_inserter(params),
                   stringValue);
    return std::make_shared<LexicalFunction>(
        LexicalFunction(params, args[1], env));
}

SchemeExpr evalVisitor::evalOr(const SchemeArgs& args, envPointer env) const
{
    for (const SchemeExpr& e : args) {
        auto evalled = eval(e, env);
        try {
            if (boolValue(evalled)) return true;
            // else ignore #f
        } catch (const scheme_error&) {
            return evalled;
        }
    }
    return false;
}

SchemeExpr
evalVisitor::evalSymbol(const std::string& symbol, envPointer env) const
{
    return env->find(symbol)[symbol];
}
