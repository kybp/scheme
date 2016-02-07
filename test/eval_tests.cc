#include "gtest/gtest.h"
#include "../src/scheme.hh"

TEST(EvalTest, EvalSelfEvaluating) {
    ASSERT_EQ(3, boost::get<int>(eval(parse("3"))));
}
