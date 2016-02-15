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

TEST(Tokenizer, ProducesEmptyOutputOnEmptyInput) {
    ASSERT_TRUE(tokenize("").empty());
}

TEST(Tokenizer, DelimitsTokensWithSpaces) {
    std::vector<std::string> expected{ "1", "2", "3" };
    ASSERT_EQ(expected, tokenize("1 2 3"));
}

TEST(Tokenizer, ConsidersParensTokens) {
    std::vector<std::string> expected{ "(", "+", "1", "2", ")" };
    ASSERT_EQ(expected, tokenize("(+ 1 2)"));
}

TEST(IntegerParser, ReadsSequenceOfDigits) {
    ASSERT_EQ(123, intValue(parse("123")));
}

TEST(IntegerParser, AllowsLeadingMinusSign) {
    ASSERT_EQ(-1, intValue(parse("-1")));
}

TEST(IntegerParser, ThrowsOnLoneMinusSign) {
    bool isInteger(const std::string token);
    ASSERT_FALSE(isInteger("-"));
}

TEST(BoolParser, ReadsHashTAsTrue) {
    ASSERT_EQ(true, boolValue(parse("#t")));
}

TEST(BoolParser, ReadsHashFAsFalse) {
    ASSERT_EQ(false, boolValue(parse("#f")));
}

TEST(SymbolParser, ReadsSymbol) {
    ASSERT_EQ("foo", symbolValue(parse("foo")).string);
}

TEST(ListParser, ReadsEmptyList) {
    ASSERT_TRUE(vectorFromExpr(parse("()")).empty());
}

TEST(ListParser, ReadsOneElementList) {
    auto actual = vectorFromExpr(parse("(3)"));
    ASSERT_EQ(1, actual.size());
    ASSERT_EQ(3, intValue(actual.front()));
}

TEST(ListParser, AllowsListNesting) {
    auto actual = vectorFromExpr(parse("(length (1 2))"));
    ASSERT_EQ(2, actual.size());
    ASSERT_EQ("length", symbolValue(actual[0]).string);
    auto sublist = vectorFromExpr(actual[1]);
    ASSERT_EQ(2, sublist.size());
    ASSERT_EQ(1, intValue(sublist[0]));
    ASSERT_EQ(2, intValue(sublist[1]));
}

TEST(ListParser, ReadsDoublyNestedLists) {
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

TEST(ListParser, ThrowsOnUnmatchedCloseParen) {
    ASSERT_THROW(parse(")"), scheme_error);
}

TEST(ListParser, ThrowsOnUnmatchedOpenParen) {
    ASSERT_THROW(parse("("), scheme_error);
}

TEST(StringParser, AllowsEmptyStrings) {
    ASSERT_EQ("", stringValue(parse("\"\"")));
}

TEST(StringParser, AllowsEmbeddedSpaces) {
    ASSERT_EQ("um hi", stringValue(parse("\"um hi\"")));
}

TEST(StringParser, AllowsLiteralEmbeddedNewlines) {
    ASSERT_EQ("um\nhi", stringValue(parse("\"um\nhi\"")));
}

TEST(StringParser, WorksInSubexpression) {
    ASSERT_NO_THROW(parse("(quote \"um\")"));
}

TEST(StringParser, EmbedsDoubleQuotesWhenEscaped) {
    ASSERT_EQ("\"um\"", stringValue(parse("\"\\\"um\\\"\"")));
}

TEST(StringParser, ReadsBackslashNAsNewline) {
    ASSERT_EQ("um\nhi", stringValue(parse("\"um\\nhi\"")));
}

TEST(StringParser, ReadsBackslashTAsTab) {
    ASSERT_EQ("um\thi", stringValue(parse("\"um\\thi\"")));
}

TEST(CharacterParser, AcceptsLiteralCharactersAsNames) {
    ASSERT_EQ('h', charValue(parse("#\\h")));
}

TEST(CharacterParser, RequiresCharacterNameAfterBackslash) {
    ASSERT_THROW(parse("#\\"), scheme_error);
}

TEST(CharacterParser, AcceptsNewlineAsSymbolicName) {
    ASSERT_EQ('\n', charValue(parse("#\\Newline")));
}

TEST(CharacterParser, AcceptsSpaceAsSymbolicName) {
    ASSERT_EQ(' ', charValue(parse("#\\Space")));
}

TEST(CharacterParser, AcceptsTabAsSymbolicName) {
    ASSERT_EQ('\t', charValue(parse("#\\Tab")));
}

TEST(CharacterParser, ThrowsOnUndefinedSymbolicName) {
    ASSERT_THROW(parse("#\\um"), scheme_error);
}

TEST(CharacterParser, WorksInSubexpression) {
    ASSERT_NO_THROW(parse("(quote #\\a)"));
}

TEST(CommentParser, IgnoresFromSemicolonOn) {
    ASSERT_NO_THROW(parse("1;)"));
    ASSERT_THROW(parse("(;)"), scheme_error);
}

TEST(CommentParser, StopsIgnoringAfterNewline) {
    ASSERT_NO_THROW(parse("(;)))\n)"));
}

TEST(QuoteReadMacro, ProducesQuoteForm) {
    ASSERT_EQ(parse("(quote foo)"), parse("'foo"));
}

TEST(QuasiquoteReadMacro, ProducesQuasiquoteForm) {
    ASSERT_EQ(parse("(quasiquote foo)"), parse("`foo"));
}

TEST(UnquoteReadMacro, ProducesUnquoteForm) {
    ASSERT_EQ(parse("(unquote foo)"), parse(",foo"));
}

TEST(UnquoteReadMacro, WorksInQuasiquote) {
    ASSERT_EQ(parse("(quasiquote (unquote foo))"), parse("`,foo"));
}

TEST(UnquoteReadMacro, WorksInSubform) {
    ASSERT_EQ(parse("(quasiquote (1 2 (unquote (+ 1 2))))"),
              parse("`(1 2 ,(+ 1 2))"));
}

TEST(UnquoteSplicingReadMacro, ProducesUnquoteSplicingForm) {
    ASSERT_EQ(parse("(unquote-splicing foo)"), parse(",@foo"));
}
