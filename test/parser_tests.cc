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
    ASSERT_EQ("foo", symbolValue(parse("foo")).string);
}

TEST(ParserTest, ParseEmptyList) {
    ASSERT_TRUE(vectorFromExpr(parse("()")).empty());
}

TEST(ParserTest, ParseOneElementList) {
    auto actual = vectorFromExpr(parse("(3)"));
    ASSERT_EQ(1, actual.size());
    ASSERT_EQ(3, intValue(actual.front()));
}

TEST(ParserTest, ParseNestedList) {
    auto actual = vectorFromExpr(parse("(length (1 2))"));
    ASSERT_EQ(2, actual.size());
    ASSERT_EQ("length", symbolValue(actual[0]).string);
    auto sublist = vectorFromExpr(actual[1]);
    ASSERT_EQ(2, sublist.size());
    ASSERT_EQ(1, intValue(sublist[0]));
    ASSERT_EQ(2, intValue(sublist[1]));
}

TEST(ParserTest, ParseDoublyNestedList) {
    auto first = vectorFromExpr(parse("(1 (2 (3)))"));
    ASSERT_EQ(2, first.size());
    ASSERT_EQ(1, intValue(first[0]));
    auto second = vectorFromExpr(first[1]);
    ASSERT_EQ(2, second.size());
    ASSERT_EQ(2, intValue(second[0]));
    auto third = vectorFromExpr(second[1]);
    ASSERT_EQ(1, third.size());
    ASSERT_EQ(3, intValue(third[0]));
}

TEST(ParserTest, UnmatchedCloseParensThrows) {
    ASSERT_THROW(parse(")"), scheme_error);
}

TEST(ParserTest, UnmatchedOpenParenThrows) {
    ASSERT_THROW(parse("("), scheme_error);
}

TEST(ParserTest, EmptyString) {
    ASSERT_EQ("", stringValue(parse("\"\"")));
}

TEST(ParserTest, StringWithSpaces) {
    ASSERT_EQ("um hi", stringValue(parse("\"um hi\"")));
}

TEST(ParserTest, MultilineStringLiteral) {
    ASSERT_EQ("um\nhi", stringValue(parse("\"um\nhi\"")));
}

TEST(ParserTest, MultipleStringsInAList) {
    ASSERT_NO_THROW(parse("(quote \"um\" \"hi\")"));
}

TEST(ParserTest, CharacterLiteral) {
    ASSERT_EQ('h', charValue(parse("#\\h")));
}

TEST(ParserTest, UnfinishedCharacterLiteralThrows) {
    ASSERT_THROW(parse("#\\"), scheme_error);
}

TEST(ParserTest, NewlineSymbolicName) {
    ASSERT_EQ('\n', charValue(parse("#\\Newline")));
}

TEST(ParserTest, SpaceSymbolicName) {
    ASSERT_EQ(' ', charValue(parse("#\\Space")));
}

TEST(ParserTest, TabSymbolicName) {
    ASSERT_EQ('\t', charValue(parse("#\\Tab")));
}

TEST(ParserTest, UnrecognizedSymbolicNameThrows) {
    ASSERT_THROW(parse("#\\um"), scheme_error);
}
