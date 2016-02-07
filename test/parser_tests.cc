#include "gtest/gtest.h"
#include "../src/scheme.hh"

TEST(ParserTest, EmptyInputGivesNoTokens) {
    ASSERT_TRUE(tokenize("").empty());
}

TEST(ParserTest, SpacesDelimitTokens) {
    std::vector<std::string> expectedTokens{ "1", "2", "3" };
    ASSERT_EQ(expectedTokens, tokenize("1 2 3"));
}

TEST(ParserTest, ParensAreTokens) {
    std::vector<std::string> expectedTokens{ "(", "+", "1", "2", ")" };
    ASSERT_EQ(expectedTokens, tokenize("(+ 1 2)"));
}
