#include <deque>
#include <string>
#include "gtest/gtest.h"
#include "parser.hh"
#include "scheme_types.hh"

std::vector<std::string> tokenize(const std::string& string)
{
    std::vector<std::string> tokens;
    std::istringstream in(string);
    std::string token;
    in >> std::noskipws;
    while (readToken(in, token)) tokens.push_back(token);
    return tokens;
}

TEST(TokenizerTest, EmptyInputGivesNoTokens) {
    ASSERT_TRUE(tokenize("").empty());
}

TEST(TokenizerTest, SpacesDelimitTokens) {
    std::vector<std::string> expected{ "1", "2", "3" };
    ASSERT_EQ(expected, tokenize("1 2 3"));
}

TEST(TokenizerTest, ParensAreTokens) {
    std::vector<std::string> expected{ "(", "+", "1", "2", ")" };
    ASSERT_EQ(expected, tokenize("(+ 1 2)"));
}

TEST(ParserTest, ParsesInteger) {
    ASSERT_EQ(3, intValue(parse("3")));
}

TEST(ParserTest, ParseNegativeInteger) {
    ASSERT_EQ(-1, intValue(parse("-1")));
}

TEST(ParserTest, MinusSignIsNotNumber) {
    bool isInteger(const std::string token);
    ASSERT_FALSE(isInteger("-"));
}

TEST(ParserTest, ParseTrue) {
    ASSERT_EQ(true, boolValue(parse("#t")));
}

TEST(ParserTest, ParseFalse) {
    ASSERT_EQ(false, boolValue(parse("#f")));
}

TEST(ParserTest, ParsesSymbol) {
    ASSERT_EQ("foo", stringValue(parse("foo")));
}

TEST(ParserTest, ParseEmptyList) {
    ASSERT_TRUE(listValue(parse("()")).empty());
}

TEST(ParserTest, ParseOneElementList) {
    auto actual = listValue(parse("(3)"));
    ASSERT_EQ(1, actual.size());
    ASSERT_EQ(3, intValue(actual.front()));
}

TEST(ParserTest, ParseNestedList) {
    auto actual = listValue(parse("(length (1 2))"));
    ASSERT_EQ(2, actual.size());
    ASSERT_EQ("length", stringValue(actual[0]));
    auto sublist = listValue(actual[1]);
    ASSERT_EQ(2, sublist.size());
    ASSERT_EQ(1, intValue(sublist[0]));
    ASSERT_EQ(2, intValue(sublist[1]));
}

TEST(ParserTest, ParseDoublyNestedList) {
    auto first = listValue(parse("(1 (2 (3)))"));
    ASSERT_EQ(2, first.size());
    ASSERT_EQ(1, intValue(first[0]));
    auto second = listValue(first[1]);
    ASSERT_EQ(2, second.size());
    ASSERT_EQ(2, intValue(second[0]));
    auto third = listValue(second[1]);
    ASSERT_EQ(1, third.size());
    ASSERT_EQ(3, intValue(third[0]));
}

TEST(ParserTest, UnmatchedCloseParensThrows) {
    ASSERT_THROW(parse(")"), scheme_error);
}

TEST(ParserTest, UnmatchedOpenParenThrows) {
    ASSERT_THROW(parse("("), scheme_error);
}
