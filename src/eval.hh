#ifndef EVAL_HH
#define EVAL_HH

#include <deque>
#include <string>
#include <vector>
#include <boost/variant.hpp>
#include "parser.hh"
#include "scheme_types.hh"

typedef std::vector<SchemeExpr> SchemeArgs;
SchemeExpr eval(SchemeExpr e, std::shared_ptr<SchemeEnvironment> env);

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
    std::vector<std::string> params;
    SchemeExpr body;
    std::shared_ptr<SchemeEnvironment> env;
public:
    LexicalFunction(std::vector<std::string> params, SchemeExpr body,
                    std::shared_ptr<SchemeEnvironment> env)
        : params(params), body(body), env(env)
    {}

    virtual SchemeExpr operator()(const SchemeArgs& args) override {
        auto execEnv = std::make_shared<SchemeEnvironment>(
            SchemeEnvironment(params, args, env));
        return eval(body, execEnv);
    }
};

inline SchemeArgs tail(const SchemeList& list) {
    SchemeArgs result;
    std::copy(list.begin() + 1, list.end(), back_inserter(result));
    return result;
}

class evalVisitor : public boost::static_visitor<SchemeExpr> {
    using envPointer = std::shared_ptr<SchemeEnvironment>;
    envPointer env;
    SchemeExpr evalAnd(const SchemeArgs& args, envPointer env) const;
    SchemeExpr evalDefine(const SchemeArgs& args, envPointer env) const;
    SchemeExpr evalFuncall(const SchemeExpr& car, const SchemeArgs& args,
                           envPointer env) const;
    SchemeExpr evalIf(const SchemeArgs& args, envPointer env) const;
    SchemeExpr evalLambda(const SchemeArgs& args, envPointer env) const;
    SchemeExpr evalOr(const SchemeArgs& args, envPointer env) const;
    SchemeExpr evalSymbol(const std::string& symbol, envPointer env) const;
public:
    evalVisitor(std::shared_ptr<SchemeEnvironment> env) : env(env) {}

    SchemeExpr operator()(int i) const {
        return i;
    }

    SchemeExpr operator()(bool b) const {
        return b;
    }

    SchemeExpr operator()(const std::string& symbol) const {
        return evalSymbol(symbol, env);
    }

    SchemeExpr operator()(const std::shared_ptr<SchemeFunction>& fn) const {
        return fn;
    }

    SchemeExpr operator()(const SchemeList& list) const {
        std::ostringstream carStream;
        carStream << list[0];
        std::string carStr = carStream.str();
        SchemeArgs args = tail(list);
             if (carStr == "quote")  return args.front();
        else if (carStr == "and")    return evalAnd(args, env);
        else if (carStr == "if")     return evalIf(args, env);
        else if (carStr == "define") return evalDefine(args, env);
        else if (carStr == "lambda") return evalLambda(args, env);
        else if (carStr == "or")     return evalOr(args, env);
        else {
            SchemeExpr car = eval(list.front(), env);
            return evalFuncall(car, args, env);
        }
    }
};

inline SchemeExpr
evalVisitor::evalDefine(const SchemeArgs& args, envPointer env) const
{
    if (args.size() == 2) {
        auto var = stringValue(args[0]);
        (*env)[var] = eval(args[1], env);
        return args.front();
    } else {
        std::ostringstream error;
        error << "define requires two arguments, given " << args.size();
        throw scheme_error(error);
    }
}

inline SchemeExpr eval(SchemeExpr e, std::shared_ptr<SchemeEnvironment> env)
{
    return boost::apply_visitor(evalVisitor(env), e);
}

#endif
