#ifndef EVAL_HH
#define EVAL_HH

#include <deque>
#include <string>
#include <vector>
#include <boost/variant.hpp>
#include "parser.hh"
#include "scheme_types.hh"

typedef std::vector<SchemeExpr> SchemeArgs;

class evalVisitor : public boost::static_visitor<SchemeExpr> {
    SchemeEnvironment& env;
public:
    evalVisitor(SchemeEnvironment& env) : env(env) {}

    SchemeExpr operator()(int i) const {
        return i;
    }

    SchemeExpr operator()(bool b) const {
        return b;
    }

    SchemeExpr operator()(const std::string& symbol) const {
        if (env.find(symbol) == env.end()) {
            std::ostringstream error;
            error << "Undefined symbol: " << symbol;
            throw scheme_error(error.str());
        } else {
            return env[symbol];
        }
    }

    SchemeExpr operator()(const std::shared_ptr<SchemeFunction>& fn) const {
        return fn;
    }

    SchemeExpr operator()(const std::deque<SchemeExpr>& list) const {
        const auto& car = stringValue(list[0]);
        if (car == "quote") {
            return list[1];
        }
        else if (car == "if") {
            auto pred = boost::apply_visitor(evalVisitor(env), list[1]);
            if (boolValue(pred)) {
                return list[2];
            } else {
                return list[3];
            }
        } else if (car == "define") {
            auto var = stringValue(list[1]);
            env[var] = boost::apply_visitor(evalVisitor(env), list[2]);
            return list[1];
        } else {
            auto car = boost::apply_visitor(evalVisitor(env), list[0]);
            auto proc = functionPointer(car);
            auto evalVisitorArg = [this](SchemeExpr e)
                { return boost::apply_visitor(evalVisitor(env), e); };
            SchemeArgs args;
            std::transform(list.begin() + 1, list.end(),
                           back_inserter(args), evalVisitorArg );
            return proc->fn(args);
        }
    }
};

inline SchemeExpr eval(SchemeExpr e, SchemeEnvironment& env)
{
    return boost::apply_visitor(evalVisitor(env), e);
}

#endif
