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
evalVisitor::evalBegin(const SchemeArgs& args, envPointer env) const
{
    if (args.empty()) throw scheme_error("Empty begin form");

    SchemeExpr result;
    for (const auto& expr : args) {
        result = eval(expr, env);
    }

    return result;
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
    auto procArgs = vectorFromExpr(args[0]);
    std::vector<SchemeSymbol> params;
    bool hasRestParam = false;

    for (std::size_t i = 0; i < procArgs.size(); ++i) {
        if (symbolValue(procArgs[i]).string != "&rest") {
            params.push_back(symbolValue(procArgs[i]));
        } else if (i == procArgs.size() - 2) { // only one rest parameter
            hasRestParam = true;
            params.push_back(symbolValue(procArgs[i + 1]));
            break;
        } else {
            throw scheme_error("Illegal rest parameter in lambda list");
        }
    }

    auto bodyVector = args;
    bodyVector[0] = parse("begin");
    auto bodyExpr = consFromVector(bodyVector);

    return std::make_shared<LexicalFunction>(
        LexicalFunction(params, bodyExpr, hasRestParam, env));
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
evalVisitor::evalQuasiquote(const SchemeArgs& args, envPointer env) const
{
    if (args.size() != 1) {
        std::ostringstream error;
        error << "quasiquote requires one argument, passed " << args.size();
        throw scheme_error(error);
    } else {
        SchemeCons cons;
        try { cons = consValue(args.front()); }
        catch (const scheme_error&) { return args.front(); }
            
        std::ostringstream carStrStream;
        carStrStream << cons.first;
        std::string carStr = carStrStream.str();
        if (carStr == "unquote-splicing") {
            throw scheme_error("Invalid splice in quasiquote");
        } else if (carStr == "unquote") {
            auto consVec = vectorFromCons(cons);
            if (consVec.size() != 2) {
                std::ostringstream error;
                error << "Unquote requires one argument, passed ";
                error << args.size();
                throw scheme_error(error);
            } else {
                return eval(consVec[1], env);
            }
        } else {
            try {
                auto subCons = consValue(car(cons));
                std::ostringstream subCar;
                subCar << car(subCons);
                if (subCar.str() == "unquote-splicing") {
                    return append(eval(car(consValue(cdr(subCons))), env),
                                  evalQuasiquote({cdr(cons)}, env));
                }
            } catch (const scheme_error&) {
            } // not unquote-splicing, so treat it like normal
            auto carVal = evalQuasiquote({car(cons)}, env);
            auto cdrVal = evalQuasiquote({cdr(cons)}, env);
            return SchemeCons(carVal, cdrVal);
        }
    }
}

SchemeExpr evalVisitor::evalQuote(const SchemeArgs& args) const
{
    if (args.size() != 1) {
        std::ostringstream error;
        error << "quote requires one argument, passed " << args.size();
        throw scheme_error(error);
    } else {
        return args.front();
    }
}

std::istream& evalStream(std::istream& in,
                         std::shared_ptr<SchemeEnvironment> env)
{
    SchemeExpr expr;
    while (readSchemeExpr(in, expr)) eval(expr, env);
    return in;
}
