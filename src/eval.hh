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

class evalVisitor : public boost::static_visitor<SchemeExpr> {
    std::shared_ptr<SchemeEnvironment> env;
public:
    evalVisitor(std::shared_ptr<SchemeEnvironment> env) : env(env) {}

    SchemeExpr operator()(int i) const {
        return i;
    }

    SchemeExpr operator()(bool b) const {
        return b;
    }

    SchemeExpr operator()(const std::string& symbol) const {
        SchemeEnvironment *definingEnv = env->find(symbol);

        if (definingEnv == nullptr) {
            std::ostringstream error;
            error << "Undefined symbol: " << symbol;
            throw scheme_error(error);
        } else {
            return (*definingEnv)[symbol];
        }
    }

    SchemeExpr operator()(const std::shared_ptr<SchemeFunction>& fn) const {
        return fn;
    }

    SchemeExpr operator()(const SchemeList& list) const {
        std::ostringstream carStream;
        carStream << list[0];
        std::string car = carStream.str();
        if (car == "quote") {
            return list[1];
        }
        else if (car == "if") {
            if (list.size() != 4) {
                std::ostringstream error;
                error << "if requires exactly 3 arguments, ";
                error << "passed " << list.size();
                throw scheme_error(error);
            }
            auto pred = eval(list[1], env);
            if (boolValue(pred)) {
                return eval(list[2], env);
            } else {
                return eval(list[3], env);
            }
        } else if (car == "define") {
            auto var = stringValue(list[1]);
            // The problem might be right here? Or maybe in
            // LexicalFunction's constructor. Anyway, I think the
            // issue is that at some point an environment we meant to
            // pass by reference is getting copied
            (*env)[var] = eval(list[2], env);
            return list[1];
        } else if (car == "lambda") {
            std::vector<std::string> params;
            auto args = listValue(list[1]);
            std::transform(args.begin(), args.end(), back_inserter(params),
                           stringValue);
            return std::make_shared<LexicalFunction>(
                LexicalFunction(params, list[2], env));
        } else {                // function call
            auto car = eval(list[0], env);
            auto evalVisitorArg = [this](SchemeExpr e)
                { return eval(e, env); };
            SchemeArgs args;
            std::transform(list.begin() + 1, list.end(),
                           back_inserter(args), evalVisitorArg );
            return (*functionPointer(car))(args);
        }
    }
};

inline SchemeExpr eval(SchemeExpr e, std::shared_ptr<SchemeEnvironment> env)
{
    return boost::apply_visitor(evalVisitor(env), e);
}

#endif
