#include "gtest/gtest.h"
#include "builtins.hh"
#include "scheme.hh"

TEST(BuiltinTest, Addition) {
    ASSERT_EQ(3, intValue(eval(parse("(+ 1 2)"))));
}

TEST(BuiltinTest, AdditionIdentity) {
    ASSERT_EQ(0, intValue(eval(parse("(+)"))));
}

TEST(BuiltinTest, AbsPositiveInput) {
    ASSERT_EQ(1, intValue(eval(parse("(abs 1)"))));
}

TEST(BuiltinTest, AbsNegativeInput) {
    ASSERT_EQ(1, intValue(eval(parse("(abs -1)"))));
}

TEST(BuiltinTest, SubtractNoArgsThrows) {
    ASSERT_THROW(eval(parse("(-)")), scheme_error);
}

TEST(BuiltinTest, SubtractOneArgNegates) {
    ASSERT_EQ(-1, intValue(eval(parse("(- 1)"))));
}
