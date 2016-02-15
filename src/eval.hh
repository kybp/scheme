#ifndef EVAL_HH
#define EVAL_HH

#include <deque>
#include <iostream>
#include <string>
#include <vector>
#include <boost/variant.hpp>
#include "parser.hh"
#include "scheme_types.hh"

typedef std::vector<SchemeExpr> SchemeArgs;
SchemeExpr eval(SchemeExpr e, std::shared_ptr<SchemeEnvironment> env);
std::istream& evalStream(std::istream&, std::shared_ptr<SchemeEnvironment>);

struct SchemeFunction {
    virtual SchemeExpr operator()(const SchemeArgs& args) = 0;
};

class PrimitiveFunction : public SchemeFunction {
    std::function<SchemeExpr(SchemeArgs)> fn;
public:
    PrimitiveFunction(std::function<SchemeExpr(const SchemeArgs&)> fn)//,
        : fn(fn)
        {}
    virtual SchemeExpr operator()(const SchemeArgs& args) override {
        return fn(args);
    }
};

class LexicalFunction : public SchemeFunction {
    std::vector<SchemeSymbol> params;
    SchemeExpr body;
    bool hasRestParam;
    std::shared_ptr<SchemeEnvironment> env;
public:
    LexicalFunction(std::vector<SchemeSymbol> params, SchemeExpr body,
                    bool hasRestParam, std::shared_ptr<SchemeEnvironment> env)
        : params(params), body(body), hasRestParam(hasRestParam), env(env)
    {}

    virtual SchemeExpr operator()(const SchemeArgs& args) override {
        auto execEnv = std::make_shared<SchemeEnvironment>(
            SchemeEnvironment(params, args, hasRestParam, env));
        return eval(body, execEnv);
    }
};

class evalVisitor : public boost::static_visitor<SchemeExpr> {
    using envPointer = std::shared_ptr<SchemeEnvironment>;
    envPointer env;
    SchemeExpr evalAnd(const SchemeArgs& args, envPointer env) const;
    SchemeExpr evalBegin(const SchemeArgs& args, envPointer env) const;
    SchemeExpr evalDefine(const SchemeArgs& args, envPointer env) const;
    SchemeExpr evalFuncall(const SchemeExpr& car, const SchemeArgs& args,
                           envPointer env) const;
    SchemeExpr evalIf(const SchemeArgs& args, envPointer env) const;
    SchemeExpr evalLambda(const SchemeArgs& args, envPointer env) const;
    SchemeExpr evalOr(const SchemeArgs& args, envPointer env) const;
    SchemeExpr evalSet(const SchemeArgs& args, envPointer env) const;
    SchemeExpr evalSymbol(const SchemeSymbol& symbol, envPointer env) const;
    SchemeExpr evalQuasiquote(const SchemeArgs& args, envPointer env) const;
    SchemeExpr evalQuote(const SchemeArgs& args) const;
public:
    evalVisitor(std::shared_ptr<SchemeEnvironment> env) : env(env) {}

    SchemeExpr operator()(bool b) const {
        return b;
    }

    SchemeExpr operator()(char c) const {
        return c;
    }

    SchemeExpr operator()(int i) const {
        return i;
    }

    SchemeExpr operator()(const std::string& string) const {
        return string;
    }

    SchemeExpr operator()(const SchemeSymbol& symbol) const {
        return evalSymbol(symbol, env);
    }

    SchemeExpr operator()(const std::shared_ptr<SchemeFunction>& fn) const {
        return fn;
    }

    SchemeExpr operator()(const Nil&) const {
        throw scheme_error("Missing function in ()");
    }

    SchemeExpr operator()(const SchemeCons& cons) const {
        std::ostringstream carStream;
        carStream << car(cons);
        std::string carStr = carStream.str();
        SchemeArgs args = vectorFromExpr(cdr(cons));
             if (carStr == "and")        return evalAnd(args, env);
        else if (carStr == "begin")      return evalBegin(args, env);
        else if (carStr == "if")         return evalIf(args, env);
        else if (carStr == "define")     return evalDefine(args, env);
        else if (carStr == "lambda")     return evalLambda(args, env);
        else if (carStr == "quasiquote") return evalQuasiquote(args, env);
        else if (carStr == "quote")      return evalQuote(args);
        else if (carStr == "or")         return evalOr(args, env);
        else if (carStr == "set!")       return evalSet(args, env);
        else if (carStr == "unquote") {
            throw scheme_error("Unquote outside of quasiquote");
        } else {
            SchemeExpr op = eval(car(cons), env);
            return evalFuncall(op, args, env);
        }
    }
};

inline SchemeExpr
evalVisitor::evalDefine(const SchemeArgs& args, envPointer env) const
{
    if (args.size() == 2) {
        auto var = symbolValue(args[0]);
        (*env)[var.string] = eval(args[1], env);
        return args.front();
    } else {
        std::ostringstream error;
        error << "define requires two arguments, passed " << args.size();
        throw scheme_error(error);
    }
}

inline SchemeExpr
evalVisitor::evalSet(const SchemeArgs& args, envPointer env) const
{
    if (args.size() != 2) {
        std::ostringstream error;
        error << "set! requires two arguments, passed " << args.size();
        throw scheme_error(error);
    } else {
        auto var = symbolValue(args[0]);
        auto definingEnv = env->find(var.string);
        if (definingEnv == nullptr) {
            std::ostringstream error;
            error << "Undefined symbol: " << var;
            throw scheme_error(error);
        } else {
            (*definingEnv)[var.string] = eval(args[1], env);
            return var;
        }
    }
}

inline SchemeExpr
evalVisitor::evalSymbol(const SchemeSymbol& symbol, envPointer env) const
{
    envPointer definingEnv = env->find(symbol.string);
    if (definingEnv) {
        return (*definingEnv)[symbol.string];
    } else {
        std::ostringstream error;
        error << "Undefined symbol: " << symbol;
        throw scheme_error(error);
    }
}

inline SchemeExpr eval(SchemeExpr e, std::shared_ptr<SchemeEnvironment> env)
{
    return boost::apply_visitor(evalVisitor(env), e);
}

#endif
