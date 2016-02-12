#include <sstream>
#include "gtest/gtest.h"
#include "builtins.hh" // for scheme::cons
#include "parser.hh"
#include "printer.hh"
#include "scheme_types.hh"

TEST(PrintTest, PrintInteger) {
    std::ostringstream s;
    s << parse("3");
    ASSERT_EQ("3", s.str());
}

TEST(PrintTest, PrintSymbol) {
    std::ostringstream s;
    s << parse("foo");
    ASSERT_EQ("foo", s.str());
}

TEST(ListTests, EmptyList) {
    std::ostringstream s;
    s << parse("()");
    ASSERT_EQ("()", s.str());
}

TEST(ListTests, OneElementList) {
    std::ostringstream s;
    s << parse("(1)");
    ASSERT_EQ("(1)", s.str());
}

TEST(ListTests, MultiElementList) {
    std::ostringstream s;
    s << parse("(1 2 3)");
    ASSERT_EQ("(1 2 3)", s.str());
}

TEST(ListTests, InsertSpacesBeforeParens) {
    std::ostringstream s;
    s << parse("(1(2)3)");
    ASSERT_EQ("(1 (2) 3)", s.str());
}

TEST(ListTests, NestedList) {
    std::ostringstream s;
    s << parse("(foo (bar (baz)))");
    ASSERT_EQ("(foo (bar (baz)))", s.str());
}

TEST(ListTests, DottedList) {
    std::ostringstream s;
    s << eval(parse("(cons 1 2)"));
    ASSERT_EQ("(1 . 2)", s.str());
}

TEST(StringTests, String) {
    std::ostringstream s;
    s << parse("\"um\"");
    ASSERT_EQ("\"um\"", s.str());
}

TEST(StringTests, EscapedQuotesPrintEscaped) {
    std::ostringstream s;
    s << parse("\"\\\"um\\\"\"");
    ASSERT_EQ("\"\\\"um\\\"\"", s.str());
}

TEST(CharacterTests, Character) {
    std::ostringstream s;
    s << parse("#\\h");
    ASSERT_EQ("#\\h", s.str());
}

TEST(CharacterTests, WhitespaceUsesSymbolicName) {
    std::ostringstream s;
    s << parse("#\\Space");
    ASSERT_EQ("#\\Space", s.str());
}
