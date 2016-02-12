#include "gtest/gtest.h"
#include "builtins.hh"          // for standard environment evaluation
#include "eval.hh"
#include "scheme_types.hh"

TEST(EvalTest, EvalSelfEvaluating) {
    ASSERT_EQ(3, intValue(eval(parse("3"))));
}

TEST(EvalTest, UndefinedVariableThrows) {
    ASSERT_THROW(eval(parse("um")), scheme_error);
}

TEST(EvalTest, UnquotedEmptyListThrows) {
    ASSERT_THROW(eval(parse("()")), scheme_error);
}

TEST(QuoteTest, UndefinedSymbolReturnedIntact) {
    ASSERT_EQ("foo", stringValue(eval(parse("(quote foo)"))));
}

TEST(QuoteTest, NoArgsThrows) {
    ASSERT_THROW(eval(parse("(quote)")), scheme_error);
}

TEST(QuoteTest, MoreThanOneArgThrows) {
    ASSERT_THROW(eval(parse("(quote 1 2)")), scheme_error);
}

TEST(DefineTest, DefineVariable) {
    auto env = std::make_shared<SchemeEnvironment>(SchemeEnvironment());
    eval(parse("(define x 2)"), env);
    ASSERT_EQ(2, intValue(eval(parse("x"), env)));
}

TEST(DefineTest, LessThanTwoArgsThrows) {
    ASSERT_THROW(eval(parse("(define)")), scheme_error);
    ASSERT_THROW(eval(parse("(define x)")), scheme_error);
}

TEST(DefineTest, MoreThanTwoArgsThrows) {
    ASSERT_THROW(eval(parse("(define foo 1 2)")), scheme_error);
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

TEST(LambdaTest, RestParameter) {
    auto env = standardEnvironment();
    eval(parse("(define num-args (lambda (&rest rest) (length rest)))"), env);
    ASSERT_EQ(3, intValue(eval(parse("(num-args 1 2 3)"), env)));
}

TEST(LambdaTest, EmptyRestParameter) {
    auto env = standardEnvironment();
    eval(parse("(define f (lambda (&rest args) (length args)))"), env);
    ASSERT_EQ(0, intValue(eval(parse("(f)"), env)));
}

TEST(LambdaTest, RestParameterWithRequiredParameters) {
    auto env = standardEnvironment();
    eval(parse("(define f (lambda (x &rest args) (cons x args)))"), env);
    ASSERT_EQ(3, intValue(eval(parse("(length (f 1 2 3))"), env)));
    ASSERT_THROW(eval(parse("(f)"), env), scheme_error);
}

TEST(LambdaTest, ReturnValueOfLastExprInBody) {
    ASSERT_EQ(3, intValue(eval(parse("((lambda () 1 2 3))"))));
}

TEST(OrTest, NoArgsIsFalse) {
    ASSERT_FALSE(boolValue(eval(parse("(or)"))));
}

TEST(OrTest, AllArgsFalseIsFalse) {
    ASSERT_FALSE(boolValue(eval(parse("(or #f #f #f)"))));
}

TEST(OrTest, AnyTrueArgsIsTrue) {
    ASSERT_TRUE(boolValue(eval(parse("(or #f #t #f)"))));
    ASSERT_TRUE(boolValue(eval(parse("(or #t #t)"))));
}

TEST(OrTest, ReturnsFirstNonFalseArg) {
    ASSERT_EQ(1, intValue(eval(parse("(or #f 1 #t)"))));
}

TEST(AndTest, NoArgsIsTrue) {
    ASSERT_TRUE(boolValue(eval(parse("(and)"))));
}

TEST(AndTest, AnyFalseArgIsFalse) {
    ASSERT_FALSE(boolValue(eval(parse("(and #t #t #f #t)"))));
}

TEST(AndTest, AllTrueArgsReturnsLast) {
    ASSERT_EQ(3, intValue(eval(parse("(and 1 2 3)"))));
}

TEST(EvalStreamTest, NoErrorOnEmptyStream) {
    std::istringstream in;
    auto env = standardEnvironment();
    ASSERT_NO_THROW(evalStream(in, env));
}

TEST(EvalStreamTest, EnvironmentChangesVisible) {
    std::istringstream in("(define square (lambda (x) (* x x)))");
    auto env = standardEnvironment();
    evalStream(in, env);
    ASSERT_EQ(4, intValue(eval(parse("(square 2)"), env)));
}

TEST(SetTest, SetUndefinedVariableThrows) {
    auto env = std::make_shared<SchemeEnvironment>(SchemeEnvironment());
    ASSERT_THROW(eval(parse("(set! x 3)"), env), scheme_error);
}

TEST(SetTest, SetRedefinesVariable) {
    auto env = std::make_shared<SchemeEnvironment>(SchemeEnvironment());
    eval(parse("(define x 1)"), env);
    ASSERT_EQ(1, intValue(eval(parse("x"), env)));
    eval(parse("(set! x 2)"), env);
    ASSERT_EQ(2, intValue(eval(parse("x"), env)));
}

TEST(BeginTest, NoArgsThrows) {
    ASSERT_THROW(eval(parse("(begin)")), scheme_error);
}

TEST(BeginTest, ReturnsLastExpression) {
    ASSERT_EQ(3, intValue(eval(parse("(begin 1 2 3)"))));
}
