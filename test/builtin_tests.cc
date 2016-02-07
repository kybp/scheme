#include "gtest/gtest.h"
#include "builtins.hh"
#include "scheme.hh"

TEST(EvalTest, EvalAddition) {
    ASSERT_EQ(3, intValue(eval(parse("(+ 1 2)"))));
}

TEST(EvalTest, EvalAdditionIdentity) {
    ASSERT_EQ(0, intValue(eval(parse("(+)"))));
}

TEST(EvalTest, EvalAbsPositiveInput) {
    ASSERT_EQ(1, intValue(eval(parse("(abs 1)"))));
}

TEST(EvalTest, EvalAbsNegativeInput) {
    ASSERT_EQ(1, intValue(eval(parse("(abs -1)"))));
}
