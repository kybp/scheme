#include "gtest/gtest.h"
#include "../src/scheme.hh"

TEST(EvalTest, EvalSelfEvaluating) {
    ASSERT_EQ(3, boost::get<int>(eval(parse("3"))));
}

TEST(EvalTest, EvalQuotedSymbol) {
    ASSERT_EQ("foo", boost::get<std::string>(eval(parse("(quote foo)"))));
}

TEST(EvalTest, EvalAddition) {
    ASSERT_EQ(3, boost::get<int>(eval(parse("(+ 1 2)"))));
}

TEST(EvalTest, EvalAdditionIdentity) {
    ASSERT_EQ(0, boost::get<int>(eval(parse("(+)"))));
}

TEST(EvalTest, EvalAbsPositiveInput) {
    ASSERT_EQ(1, boost::get<int>(eval(parse("(abs 1)"))));
}

TEST(EvalTest, EvalAbsNegativeInput) {
    ASSERT_EQ(1, boost::get<int>(eval(parse("(abs -1)"))));
}

TEST(EvalTest, DefineVariable) {
    SchemeEnvironment env = standardEnvironment();
    eval(parse("(define x 2)"), env);
    ASSERT_EQ(2, boost::get<int>(eval(parse("x"), env)));
}
