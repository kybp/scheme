#include "gtest/gtest.h"
#include "../src/scheme.hh"

TEST(ParserTest, EmptyInputGivesNoTokens) {
    ASSERT_TRUE(tokenize("").empty());
}
