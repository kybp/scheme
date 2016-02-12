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

TEST(IntegerTest, Unsigned) {
    ASSERT_EQ(3, intValue(parse("3")));
}

TEST(IntegerTest, NegativeInteger) {
    ASSERT_EQ(-1, intValue(parse("-1")));
}

TEST(IntegerTest, MinusSignIsNotNumber) {
    bool isInteger(const std::string token);
    ASSERT_FALSE(isInteger("-"));
}

TEST(BoolTest, True) {
    ASSERT_EQ(true, boolValue(parse("#t")));
}

TEST(BoolTest, False) {
    ASSERT_EQ(false, boolValue(parse("#f")));
}

TEST(BoolTest, Symbol) {
    ASSERT_EQ("foo", symbolValue(parse("foo")).string);
}

TEST(ListTest, EmptyList) {
    ASSERT_TRUE(vectorFromExpr(parse("()")).empty());
}

TEST(ListTest, OneElementList) {
    auto actual = vectorFromExpr(parse("(3)"));
    ASSERT_EQ(1, actual.size());
    ASSERT_EQ(3, intValue(actual.front()));
}

TEST(ListTest, NestedList) {
    auto actual = vectorFromExpr(parse("(length (1 2))"));
    ASSERT_EQ(2, actual.size());
    ASSERT_EQ("length", symbolValue(actual[0]).string);
    auto sublist = vectorFromExpr(actual[1]);
    ASSERT_EQ(2, sublist.size());
    ASSERT_EQ(1, intValue(sublist[0]));
    ASSERT_EQ(2, intValue(sublist[1]));
}

TEST(Test, DoublyNestedList) {
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

TEST(ListTest, UnmatchedCloseParensThrows) {
    ASSERT_THROW(parse(")"), scheme_error);
}

TEST(Test, UnmatchedOpenParenThrows) {
    ASSERT_THROW(parse("("), scheme_error);
}

TEST(StringTest, EmptyString) {
    ASSERT_EQ("", stringValue(parse("\"\"")));
}

TEST(StringTest, EmbeddedSpaces) {
    ASSERT_EQ("um hi", stringValue(parse("\"um hi\"")));
}

TEST(StringTest, EmbeddedNewline) {
    ASSERT_EQ("um\nhi", stringValue(parse("\"um\nhi\"")));
}

TEST(StringTest, MultipleStringsInAList) {
    ASSERT_NO_THROW(parse("(quote \"um\" \"hi\")"));
}

TEST(StringTest, BackslashEscapesDelimiter) {
    ASSERT_EQ("\"um\"", stringValue(parse("\"\\\"um\\\"\"")));
}

TEST(StringTest, BackslashNIsNewline) {
    ASSERT_EQ("um\nhi", stringValue(parse("\"um\\nhi\"")));
}

TEST(StringTest, BackslashTIsTab) {
    ASSERT_EQ("um\thi", stringValue(parse("\"um\\thi\"")));
}

TEST(CharacterTest, SimpleLiteral) {
    ASSERT_EQ('h', charValue(parse("#\\h")));
}

TEST(CharacterTest, UnfinishedCharacterLiteralThrows) {
    ASSERT_THROW(parse("#\\"), scheme_error);
}

TEST(CharacterTest, NewlineSymbolicName) {
    ASSERT_EQ('\n', charValue(parse("#\\Newline")));
}

TEST(CharacterTest, SpaceSymbolicName) {
    ASSERT_EQ(' ', charValue(parse("#\\Space")));
}

TEST(CharacterTest, TabSymbolicName) {
    ASSERT_EQ('\t', charValue(parse("#\\Tab")));
}

TEST(CharacterTest, UndefinedSymbolicNameThrows) {
    ASSERT_THROW(parse("#\\um"), scheme_error);
}
