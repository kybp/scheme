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

TEST(PrintTest, EmptyList) {
    std::ostringstream s;
    s << parse("()");
    ASSERT_EQ("()", s.str());
}

TEST(PrintTest, OneElementList) {
    std::ostringstream s;
    s << parse("(1)");
    ASSERT_EQ("(1)", s.str());
}

TEST(PrintTest, MultiElementList) {
    std::ostringstream s;
    s << parse("(1 2 3)");
    ASSERT_EQ("(1 2 3)", s.str());
}

TEST(PrintTest, InsertSpacesBeforeParens) {
    std::ostringstream s;
    s << parse("(1(2)3)");
    ASSERT_EQ("(1 (2) 3)", s.str());
}

TEST(PrintTest, NestedList) {
    std::ostringstream s;
    s << parse("(foo (bar (baz)))");
    ASSERT_EQ("(foo (bar (baz)))", s.str());
}

TEST(PrintTest, DottedList) {
    std::ostringstream s;
    s << eval(parse("(cons 1 2)"));
    ASSERT_EQ("(1 . 2)", s.str());
}
