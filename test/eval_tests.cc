#include "gtest/gtest.h"
#include "builtins.hh"
#include "eval.hh"
#include "scheme_types.hh"

TEST(EvalTest, EvalSelfEvaluating) {
    ASSERT_EQ(3, intValue(eval(parse("3"))));
}

TEST(EvalTest, EvalQuotedSymbol) {
    ASSERT_EQ("foo", stringValue(eval(parse("(quote foo)"))));
}

TEST(EvalTest, DefineVariable) {
    auto env = std::make_shared<SchemeEnvironment>(SchemeEnvironment());
    eval(parse("(define x 2)"), env);
    ASSERT_EQ(2, intValue(eval(parse("x"), env)));
}

TEST(IfTest, LessThanTwoArgsThrows) {
    ASSERT_THROW(eval(parse("(if)")),      scheme_error);
    ASSERT_THROW(eval(parse("(if #t)")),   scheme_error);
    ASSERT_THROW(eval(parse("(if #t 1)")), scheme_error);
}

TEST(IfTest, MoreThanTwoArgsThrows) {
    ASSERT_THROW(eval(parse("(if 1 2 3)")), scheme_error);
}

TEST(IfTest, NonBoolPredicateThrows) {
    ASSERT_THROW(eval(parse("(if 1 2 3)")), scheme_error);
}

TEST(IfTest, EvaluatesConsequentOnTrue) {
    ASSERT_TRUE(boolValue(eval(parse("(if #t (if #t #t #f) #f)"))));
}

TEST(LambdaTest, CallFunctionLiteral) {
    ASSERT_TRUE(boolValue(eval(parse("((lambda () #t))"))));
}

TEST(LambdaTest, DefineConstantFunction) {
    auto env = std::make_shared<SchemeEnvironment>(SchemeEnvironment());
    eval(parse("(define three (lambda () 3))"), env);
    ASSERT_EQ(3, intValue(eval(parse("(three)"), env)));
}
