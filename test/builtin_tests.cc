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
    ASSERT_THROW(eval(parse("(+ 1 foo 2)")), scheme_error);
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

// abs

TEST(AbsTests, AbsPositiveInput) {
    ASSERT_EQ(1, intValue(eval(parse("(abs 1)"))));
}

TEST(AbsTests, AbsNegativeInput) {
    ASSERT_EQ(1, intValue(eval(parse("(abs -1)"))));
}
