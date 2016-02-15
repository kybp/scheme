#include <sstream>
#include "gtest/gtest.h"
#include "builtins.hh" // for scheme::cons
#include "parser.hh"
#include "printer.hh"
#include "scheme_types.hh"

TEST(Printer, PrintsInteger) {
    std::ostringstream s;
    s << parse("3");
    ASSERT_EQ("3", s.str());
}

TEST(Printer, PrintsSymbol) {
    std::ostringstream s;
    s << parse("foo");
    ASSERT_EQ("foo", s.str());
}

TEST(Printer, PrintsEmptyListAsAnOpenAndCloseParen) {
    std::ostringstream s;
    s << parse("()");
    ASSERT_EQ("()", s.str());
}

TEST(Printer, PrintsSingleElementListWithNoSpaces) {
    std::ostringstream s;
    s << parse("(1)");
    ASSERT_EQ("(1)", s.str());
}

TEST(Printer, PrintsMultiElementListWithSpacesBetweenElements) {
    std::ostringstream s;
    s << parse("(1 2 3)");
    ASSERT_EQ("(1 2 3)", s.str());
}

TEST(Printer, InsertsSpacesBeforeAndAfterSublists) {
    std::ostringstream s;
    s << parse("(1(2)3)");
    ASSERT_EQ("(1 (2) 3)", s.str());
}

TEST(Printer, PrintsArbitrarilyDeeplyNestedLists) {
    std::ostringstream s;
    s << parse("(foo (bar (baz)))");
    ASSERT_EQ("(foo (bar (baz)))", s.str());
}

TEST(Printer, PrintsImproperListWithDotBeforeCdr) {
    std::ostringstream s;
    s << eval(parse("(cons 1 2)"));
    ASSERT_EQ("(1 . 2)", s.str());
}

TEST(Printer, PrintsStringWithDoubleQuotes) {
    std::ostringstream s;
    s << parse("\"um\"");
    ASSERT_EQ("\"um\"", s.str());
}

TEST(Printer, EscapesEmbeddedDoubleQuotesInStrings) {
    std::ostringstream s;
    s << parse("\"\\\"um\\\"\"");
    ASSERT_EQ("\"\\\"um\\\"\"", s.str());
}

TEST(Printer, PrintsCharacterWithLiteralSyntax) {
    std::ostringstream s;
    s << parse("#\\h");
    ASSERT_EQ("#\\h", s.str());
}

TEST(Printer, UsesSymbolicNamesForWhitespaceCharacters) {
    std::ostringstream s;
    s << parse("#\\Space");
    ASSERT_EQ("#\\Space", s.str());
}
