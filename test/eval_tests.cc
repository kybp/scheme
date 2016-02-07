#include "gtest/gtest.h"
#include "eval.hh"
#include "scheme.hh"

TEST(EvalTest, EvalSelfEvaluating) {
    SchemeEnvironment env;
    ASSERT_EQ(3, intValue(eval(parse("3"), env)));
}

TEST(EvalTest, EvalQuotedSymbol) {
    SchemeEnvironment env;
    ASSERT_EQ("foo", stringValue(eval(parse("(quote foo)"), env)));
}

TEST(EvalTest, DefineVariable) {
    SchemeEnvironment env;
    eval(parse("(define x 2)"), env);
    ASSERT_EQ(2, intValue(eval(parse("x"), env)));
}
