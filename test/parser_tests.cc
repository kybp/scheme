#include <deque>
#include <string>
#include "gtest/gtest.h"
#include "../src/scheme.hh"

TEST(TokenizerTest, EmptyInputGivesNoTokens) {
    ASSERT_TRUE(tokenize("").empty());
}

TEST(TokenizerTest, SpacesDelimitTokens) {
    std::deque<std::string> expectedTokens{ "1", "2", "3" };
    ASSERT_EQ(expectedTokens, tokenize("1 2 3"));
}

TEST(TokenizerTest, ParensAreTokens) {
    std::deque<std::string> expectedTokens{ "(", "+", "1", "2", ")" };
    ASSERT_EQ(expectedTokens, tokenize("(+ 1 2)"));
}

TEST(ParserTest, ParsesInteger) {
    ASSERT_EQ(3, boost::get<int>(parse("3")));
}

TEST(ParserTest, ParseNegativeInteger) {
    ASSERT_EQ(-1, boost::get<int>(parse("-1")));
}

TEST(ParserTest, ParsesSymbol) {
    ASSERT_EQ("foo", boost::get<std::string>(parse("foo")));
}

TEST(ParserTest, ParseEmptyList) {
    ASSERT_TRUE(boost::get<std::deque<SchemeExpr>>(parse("()")).empty());
}

TEST(ParserTest, ParseOneElementList) {
    auto actual = boost::get<std::deque<SchemeExpr>>(parse("(3)"));
    ASSERT_EQ(1, actual.size());
    ASSERT_EQ(3, boost::get<int>(actual.front()));
}

TEST(ParserTest, ParseNestedList) {
    auto actual = boost::get<std::deque<SchemeExpr>>(parse("(length (1 2))"));
    ASSERT_EQ(2, actual.size());
    ASSERT_EQ("length", boost::get<std::string>(actual[0]));
    auto sublist = boost::get<std::deque<SchemeExpr>>(actual[1]);
    ASSERT_EQ(2, sublist.size());
    ASSERT_EQ(1, boost::get<int>(sublist[0]));
    ASSERT_EQ(2, boost::get<int>(sublist[1]));
}

TEST(ParserTest, ParseDoublyNestedList) {
    auto first = boost::get<std::deque<SchemeExpr>>(parse("(1 (2 (3)))"));
    ASSERT_EQ(2, first.size());
    ASSERT_EQ(1, boost::get<int>(first[0]));
    auto second = boost::get<std::deque<SchemeExpr>>(first[1]);
    ASSERT_EQ(2, second.size());
    ASSERT_EQ(2, boost::get<int>(second[0]));
    auto third = boost::get<std::deque<SchemeExpr>>(second[1]);
    ASSERT_EQ(1, third.size());
    ASSERT_EQ(3, boost::get<int>(third[0]));
}
