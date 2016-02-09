#include "gtest/gtest.h"
#include "builtins.hh"
#include "parser.hh"
#include "scheme_types.hh"

// +

TEST(AdditionTests, BasicAddition) {
    ASSERT_EQ(3, intValue(eval(parse("(+ 1 2)"))));
}

TEST(AdditionTests, Identity) {
    ASSERT_EQ(0, intValue(eval(parse("(+)"))));
}

TEST(AdditionTests, NonIntegralThrows) {
    ASSERT_THROW(eval(parse("(+ 1 (quote foo) 2)")), scheme_error);
}

TEST(AdditionTests, ArgumentsAreEvaluated) {
    ASSERT_EQ(6, intValue(eval(parse("(+ (+ 1 2) 3)"))));
}

// -

TEST(SubtractionTests, NoArgsThrows) {
    ASSERT_THROW(eval(parse("(-)")), scheme_error);
}

TEST(SubtractionTests, OneArgNegates) {
    ASSERT_EQ(-1, intValue(eval(parse("(- 1)"))));
}

TEST(SubtractionTests, BasicSubtraction) {
    ASSERT_EQ(1, intValue(eval(parse("(- 3 2)"))));
}

// *

TEST(MultiplicationTest, Identity) {
    ASSERT_EQ(1, intValue(eval(parse("(*)"))));
}

TEST(MultiplicationTest, BasicMultiplication) {
    ASSERT_EQ(6, intValue(eval(parse("(* 1 2 3)"))));
}

// <

TEST(LessThanTest, LessThanTwoArgsThrows) {
    ASSERT_THROW(eval(parse("(<)")), scheme_error);
    ASSERT_THROW(eval(parse("(< 1)")), scheme_error);
}

TEST(LessThanTest, LessThanIsTrue) {
    ASSERT_TRUE(boolValue(eval(parse("(< 1 2)"))));
}

TEST(LessThanIsTrue, EqualIsFalse) {
    ASSERT_FALSE(boolValue(eval(parse("(< 2 2)"))));
}

TEST(LessThanTest, GreaterThanIsFalse) {
    ASSERT_FALSE(boolValue(eval(parse("(< 2 1)"))));
}

TEST(LessThanTest, NonIntegerThrows) {
    ASSERT_THROW(eval(parse("(< (quote a) (quote b))")), scheme_error);
}

// =

TEST(EqualTest, LessThanTwoArgsThrows) {
    ASSERT_THROW(eval(parse("(=)")), scheme_error);
    ASSERT_THROW(eval(parse("(= 1)")), scheme_error);
}

TEST(EqualTest, NonIntegerThrows) {
    ASSERT_THROW(eval(parse("(= 2 (quote um))")), scheme_error);
}

TEST(EqualTest, SameIntegerIsTrue) {
    ASSERT_TRUE(boolValue(eval(parse("(= 2 2)"))));
}

TEST(EqualTest, DifferentIntegersIsFalse) {
    ASSERT_FALSE(boolValue(eval(parse("(= 2 2 3)"))));
}

// >

TEST(GreaterTest, LessThanTwoArgsThrows) {
    ASSERT_THROW(eval(parse("(>)")), scheme_error);
    ASSERT_THROW(eval(parse("(> 1)")), scheme_error);
}

TEST(GreaterTest, LessThanIsFalse) {
    ASSERT_FALSE(boolValue(eval(parse("(> 1 2)"))));
}

TEST(GreaterTest, EqualIsFalse) {
    ASSERT_FALSE(boolValue(eval(parse("(> 2 2)"))));
}

TEST(GreaterTest, GreaterIsTrue) {
    ASSERT_TRUE(boolValue(eval(parse("(> 3 2 1)"))));
}

// abs

TEST(AbsTests, AbsPositiveInput) {
    ASSERT_EQ(1, intValue(eval(parse("(abs 1)"))));
}

TEST(AbsTests, AbsNegativeInput) {
    ASSERT_EQ(1, intValue(eval(parse("(abs -1)"))));
}

// not

TEST(NotTests, NoArgsThrows) {
    ASSERT_THROW(eval(parse("(not)")), scheme_error);
}

TEST(NotTests, MoreThanOneArgThrows) {
    ASSERT_THROW(eval(parse("(not #t #t)")), scheme_error);
}

TEST(NotTests, NonBoolArgThrows) {
    ASSERT_THROW(eval(parse("(not 1)")), scheme_error);
    ASSERT_THROW(eval(parse("(not (quote um))")), scheme_error);
}

TEST(NotTests, NotTrueIsFalse) {
    ASSERT_FALSE(boolValue(eval(parse("(not #t)"))));
}

TEST(NotTests, NotFalseIsTrue) {
    ASSERT_TRUE(boolValue(eval(parse("(not #f)"))));
}
