#include "gtest/gtest.h"
#include "eval.hh"
#include "scheme_types.hh"

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

TEST(IfTest, LessThanTwoArgsThrows) {
    SchemeEnvironment env;
    ASSERT_THROW(eval(parse("(if)"),      env), scheme_error);
    ASSERT_THROW(eval(parse("(if #t)"),   env), scheme_error);
    ASSERT_THROW(eval(parse("(if #t 1)"), env), scheme_error);
}

TEST(IfTest, MoreThanTwoArgsThrows) {
    SchemeEnvironment env;
    ASSERT_THROW(eval(parse("(if 1 2 3)"), env), scheme_error);
}

TEST(IfTest, NonBoolPredicateThrows) {
    SchemeEnvironment env;
    ASSERT_THROW(eval(parse("(if 1 2 3)"), env), scheme_error);
}

TEST(LambdaTest, DefineConstantFunction) {
    SchemeEnvironment env;
    eval(parse("(define three (lambda () 3))"), env);
    ASSERT_EQ(3, intValue(eval(parse("(three)"), env)));
}
