#include "gtest/gtest.h"
#include "builtins.hh"          // for standard environment evaluation
#include "eval.hh"
#include "scheme_types.hh"

TEST(Eval, EvaluatesNumberToItself) {
    ASSERT_EQ(3, intValue(eval(parse("3"))));
}

TEST(Eval, EvaluatesBoolToItself) {
    ASSERT_EQ(true, boolValue(eval(parse("#t"))));
}

TEST(Eval, ThrowsOnUndefinedVariable) {
    ASSERT_THROW(eval(parse("um")), scheme_error);
}

TEST(Eval, ThrowsOnEmptyList) {
    ASSERT_THROW(eval(parse("()")), scheme_error);
}

TEST(Quote, ReturnsUnboundSymbolIntact) {
    ASSERT_EQ("foo", symbolValue(eval(parse("(quote foo)"))).string);
}

TEST(Quote, ThrowsOnNoArgs) {
    ASSERT_THROW(eval(parse("(quote)")), scheme_error);
}

TEST(Quote, ThrowsOnMoreThanOneArg) {
    ASSERT_THROW(eval(parse("(quote 1 2)")), scheme_error);
}

TEST(Define, MutatesEnvironment) {
    auto env = std::make_shared<SchemeEnvironment>(SchemeEnvironment());
    eval(parse("(define x 2)"), env);
    ASSERT_EQ(2, intValue(eval(parse("x"), env)));
}

TEST(Define, ThrowsOnLessThanTwoArgs) {
    ASSERT_THROW(eval(parse("(define)")), scheme_error);
    ASSERT_THROW(eval(parse("(define x)")), scheme_error);
}

TEST(Define, ThrowsOnMoreThanTwoArgs) {
    ASSERT_THROW(eval(parse("(define foo 1 2)")), scheme_error);
}

TEST(If, ThrowsWithLessThanThreeArgs) {
    ASSERT_THROW(eval(parse("(if)")),      scheme_error);
    ASSERT_THROW(eval(parse("(if #t)")),   scheme_error);
    ASSERT_THROW(eval(parse("(if #t 1)")), scheme_error);
}

TEST(If, ThrowsOnMoreThanTwoArgs) {
    ASSERT_THROW(eval(parse("(if #t 1 2 3)")), scheme_error);
}

TEST(If, ThrowsOnNonBooleanPredicate) {
    ASSERT_THROW(eval(parse("(if 1 2 3)")), scheme_error);
}

TEST(If, EvaluatesConsequentOnTrue) {
    ASSERT_TRUE(boolValue(eval(parse("(if #t (if #t #t #f) #f)"))));
}

TEST(Lambda, CanBeCalledDirectly) {
    ASSERT_TRUE(boolValue(eval(parse("((lambda () #t))"))));
}

TEST(Lambda, CanProduceConstantFunction) {
    auto env = std::make_shared<SchemeEnvironment>(SchemeEnvironment());
    eval(parse("(define three (lambda () 3))"), env);
    ASSERT_EQ(3, intValue(eval(parse("(three)"), env)));
}

TEST(Lambda, RestParameterBindsArgsToList) {
    auto env = standardEnvironment();
    eval(parse("(define num-args (lambda (&rest rest) (length rest)))"), env);
    ASSERT_EQ(3, intValue(eval(parse("(num-args 1 2 3)"), env)));
}

TEST(Lambda, RestParameterCanBeEmpty) {
    auto env = standardEnvironment();
    eval(parse("(define f (lambda (&rest args) (length args)))"), env);
    ASSERT_EQ(0, intValue(eval(parse("(f)"), env)));
}

TEST(Lambda, AllowsRestParameterWithRequiredParameters) {
    auto env = standardEnvironment();
    eval(parse("(define f (lambda (x &rest args) (cons x args)))"), env);
    ASSERT_EQ(3, intValue(eval(parse("(length (f 1 2 3))"), env)));
    ASSERT_THROW(eval(parse("(f)"), env), scheme_error);
}

TEST(Lambda, ReturnsValueOfLastExprInBody) {
    ASSERT_EQ(3, intValue(eval(parse("((lambda () 1 2 3))"))));
}

TEST(Or, ReturnsFalseWithoutArgs) {
    ASSERT_FALSE(boolValue(eval(parse("(or)"))));
}

TEST(Or, ReturnsFalseWithAllFalseArgs) {
    ASSERT_FALSE(boolValue(eval(parse("(or #f #f #f)"))));
}

TEST(Or, ReturnsFirstNonFalseArg) {
    ASSERT_EQ(1, intValue(eval(parse("(or #f #f 1 2 3)"))));
}

TEST(And, ReturnsTrueWithNoArgs) {
    ASSERT_TRUE(boolValue(eval(parse("(and)"))));
}

TEST(And, ReturnsFalseIfAnyArgIsFalse) {
    ASSERT_FALSE(boolValue(eval(parse("(and #t #t #f #t)"))));
}

TEST(And, ReturnsLastExpressionWithAllTrueArgs) {
    ASSERT_EQ(3, intValue(eval(parse("(and 1 2 3)"))));
}

TEST(EvalStream, DoesNotThrowOnEmptyStream) {
    std::istringstream in;
    auto env = standardEnvironment();
    ASSERT_NO_THROW(evalStream(in, env));
}

TEST(EvalStream, MutatesEnvironment) {
    std::istringstream in("(define square (lambda (x) (* x x)))");
    auto env = standardEnvironment();
    evalStream(in, env);
    ASSERT_EQ(4, intValue(eval(parse("(square 2)"), env)));
}

TEST(Set, ThrowsOnUndefinedVariable) {
    auto env = std::make_shared<SchemeEnvironment>(SchemeEnvironment());
    ASSERT_THROW(eval(parse("(set! x 3)"), env), scheme_error);
}

TEST(Set, RedefinesVariable) {
    auto env = std::make_shared<SchemeEnvironment>(SchemeEnvironment());
    eval(parse("(define x 1)"), env);
    ASSERT_EQ(1, intValue(eval(parse("x"), env)));
    eval(parse("(set! x 2)"), env);
    ASSERT_EQ(2, intValue(eval(parse("x"), env)));
}

TEST(Begin, ThrowsWithNoArgs) {
    ASSERT_THROW(eval(parse("(begin)")), scheme_error);
}

TEST(Begin, ReturnsLastExpression) {
    ASSERT_EQ(3, intValue(eval(parse("(begin 1 2 3)"))));
}

TEST(Quasiquote, SuppressesEvaluation) {
    ASSERT_EQ("foo", symbolValue(eval(parse("(quasiquote foo)"))).string);
}

TEST(Unquote, EvaluatesQuasiquotedForms) {
    ASSERT_EQ(3, intValue(eval(parse("(quasiquote (unquote (+ 1 2)))"))));
}

TEST(Unquote, WorksOnQuasiquoteSubExpressions) {
    ASSERT_EQ(6, intValue(
                  eval(eval(parse("(quasiquote (+ (unquote (+ 1 2)) 3))")))));
}

TEST(Unquote, ThrowsOutsideOfQuasiquote) {
    ASSERT_THROW(eval(parse("(unquote foo)")), scheme_error);
}

TEST(UnquoteSplicing, ThrowsOutsideOfQuasiquote) {
    ASSERT_THROW(eval(parse("(unquote-splicing (quote (1)))")), scheme_error);
}

TEST(UnquoteSplicing, ThrowsAtQuasiquoteToplevel) {
    ASSERT_THROW(eval(parse("(quasiquote (unquote-splicing (quote ())))")),
                 scheme_error);
}

TEST(UnquoteSplicing, ThrowsOnNonList) {
    ASSERT_THROW(eval(parse("(quasiquote (cons 1 (unquote-splicing 2)))")),
                 scheme_error);
}

TEST(UnquoteSplicing, SplicesAListIntoItsParent) {
    ASSERT_EQ(parse("(1 2 3)"), eval(parse("`(1 2 ,@(cons 3 '()))")));
}

