#include <vector>
#include "scheme_types.hh"

SchemeEnvironment::SchemeEnvironment(
    const std::vector<SchemeSymbol>& params,
    const std::vector<SchemeExpr>& args,
    bool hasRestParam,
    std::shared_ptr<SchemeEnvironment> outer)
    : outer(outer)
{
    if (!hasRestParam && params.size() != args.size()) {
        std::ostringstream error;
        error << "Function expected " << params.size() << " arguments, ";
        error << "passed " << args.size();
        throw scheme_error(error);
    }

    if (hasRestParam && params.size() - 1 > args.size()) {
        std::ostringstream error;
        error << "Function expected at least " << params.size() - 1;
        error << " arguments, passed " << args.size();
        throw scheme_error(error);
    }

    if (hasRestParam && args.empty()) {
        // The rest param has to be the only one, as we've already
        // checked we have enough required args
        definitions[params[0].string] = SchemeExpr(Nil::Nil);
    } else {
        for (std::size_t i = 0; i < params.size(); ++i) {
            definitions[params[i].string] = args[i];
        }
    }

    if (hasRestParam) {
        std::vector<SchemeExpr> rest(args.begin() + params.size() - 1,
                                     args.end());
        if (rest.empty()) {
            definitions[params[params.size() - 1].string] = Nil::Nil;
        } else {
            definitions[params[params.size() - 1].string] =
                consFromVector(rest);
        }
    }
}

std::vector<SchemeExpr> vectorFromCons(SchemeCons cons)
{
    std::vector<SchemeExpr> v;

    for (;;) {
        v.push_back(car(cons));
        if (SchemeExpr(Nil::Nil) == cdr(cons)) break;
        cons = consValue(cdr(cons));
    }

    return v;
}

SchemeCons consFromVector(const std::vector<SchemeExpr>& vector) {
    if (vector.empty()) {
        throw std::runtime_error("Can't convert empty vector to cons");
    }

    SchemeCons cons{ vector.back(), SchemeExpr(Nil::Nil) };
    SchemeCons prev;
    for (auto it = vector.rbegin() + 1; it != vector.rend(); ++it) {
        prev = { *it, cons };
        cons = prev;
    }

    return cons;
}
