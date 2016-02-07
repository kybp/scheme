#include "gtest/gtest.h"
#include "../src/scheme.hh"

TEST(TokenizerTest, EmptyInputGivesNoTokens) {
    ASSERT_TRUE(tokenize("").empty());
}

TEST(TokenizerTest, SpacesDelimitTokens) {
    std::vector<std::string> expectedTokens{ "1", "2", "3" };
    ASSERT_EQ(expectedTokens, tokenize("1 2 3"));
}

TEST(TokenizerTest, ParensAreTokens) {
    std::vector<std::string> expectedTokens{ "(", "+", "1", "2", ")" };
    ASSERT_EQ(expectedTokens, tokenize("(+ 1 2)"));
}

TEST(ParserTest, ParsesInteger) {
    ASSERT_EQ(3, boost::get<int>(readToken("3")));
}

TEST(ParserTest, ParsesSymbol) {
    ASSERT_EQ("foo", boost::get<std::string>(readToken("foo")));
}
