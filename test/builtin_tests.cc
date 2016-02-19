#include "gtest/gtest.h"
#include "builtins.hh"
#include "parser.hh"
#include "scheme_types.hh"

// +

TEST(Add, AddsIntegers) {
    ASSERT_EQ(3, intValue(eval(parse("(+ 1 2)"))));
}

TEST(Add, ReturnsZeroWithNoArgs) {
    ASSERT_EQ(0, intValue(eval(parse("(+)"))));
}

TEST(Add, ThrowsOnNonIntegral) {
    ASSERT_THROW(eval(parse("(+ 1 (quote foo) 2)")), scheme_error);
}

// -

TEST(Sub, ThrowsOnNoArgs) {
    ASSERT_THROW(eval(parse("(-)")), scheme_error);
}

TEST(Sub, NegatesSingleArgument) {
    ASSERT_EQ(-1, intValue(eval(parse("(- 1)"))));
}

TEST(Sub, SubtractsSecondNumberFromFirst) {
    ASSERT_EQ(1, intValue(eval(parse("(- 3 2)"))));
}

// *

TEST(Mul, ReturnsOneWithNoArgs) {
    ASSERT_EQ(1, intValue(eval(parse("(*)"))));
}

TEST(Mul, ReturnsProductOfArguments) {
    ASSERT_EQ(6, intValue(eval(parse("(* 1 2 3)"))));
}

// <

TEST(LessThan, ThrowsWithLessThanTwoArgs) {
    ASSERT_THROW(eval(parse("(<)")), scheme_error);
    ASSERT_THROW(eval(parse("(< 1)")), scheme_error);
}

TEST(LessThan, ReturnsTrueWithAscendingArgs) {
    ASSERT_TRUE(boolValue(eval(parse("(< 1 2 3)"))));
}

TEST(LessThan, ReturnsFalseWithEqualArgs) {
    ASSERT_FALSE(boolValue(eval(parse("(< 2 2)"))));
}

TEST(LessThan, ReturnsFalseWithDescendingArgs) {
    ASSERT_FALSE(boolValue(eval(parse("(< 2 1)"))));
}

TEST(LessThan, ThrowsOnNonNumericArgs) {
    ASSERT_THROW(eval(parse("(< (quote a) (quote b))")), scheme_error);
}

// =

TEST(EqualOp, ThrowsWithLessThanTwoArgs) {
    ASSERT_THROW(eval(parse("(=)")), scheme_error);
    ASSERT_THROW(eval(parse("(= 1)")), scheme_error);
}

TEST(EqualOp, ThrowsWithNonNumericArgs) {
    ASSERT_THROW(eval(parse("(= 2 (quote um))")), scheme_error);
}

TEST(EqualOp, ReturnsTrueWithEqualNumericArgs) {
    ASSERT_TRUE(boolValue(eval(parse("(= 2 2)"))));
}

TEST(EqualOp, ReturnsFalseWithNonEqualNumericArgs) {
    ASSERT_FALSE(boolValue(eval(parse("(= 2 2 3)"))));
}

// >

TEST(GreaterThan, ThrowsWithLessThanTwoArgs) {
    ASSERT_THROW(eval(parse("(>)")), scheme_error);
    ASSERT_THROW(eval(parse("(> 1)")), scheme_error);
}

TEST(GreaterThan, ReturnsFalseWithAscendingArgs) {
    ASSERT_FALSE(boolValue(eval(parse("(> 1 2)"))));
}

TEST(GreaterThan, ReturnsFalseWithEqualArgs) {
    ASSERT_FALSE(boolValue(eval(parse("(> 2 2)"))));
}

TEST(GreaterThan, ReturnsTrueWithDescendingArgs) {
    ASSERT_TRUE(boolValue(eval(parse("(> 3 2 1)"))));
}

// abs

TEST(Abs, ReturnsPositiveNumberIntact) {
    ASSERT_EQ(1, intValue(eval(parse("(abs 1)"))));
}

TEST(Abs, NegatesNegativeNumber) {
    ASSERT_EQ(1, intValue(eval(parse("(abs -1)"))));
}

TEST(Abs, ThrowsOnNonNumericArg) {
    ASSERT_THROW(eval(parse("(abs 'um)")), scheme_error);
}

TEST(Abs, ThrowsWithNoArgs) {
    ASSERT_THROW(eval(parse("(abs)")), scheme_error);
}

TEST(Abs, ThrowsWithMoreThanOneArg) {
    ASSERT_THROW(eval(parse("(abs 1 2)")), scheme_error);
}

// append

TEST(Append, ReturnsEmptyListWithNoArgs) {
    ASSERT_EQ(parse("()"), eval(parse("(append)")));
}

TEST(Append, AppendsTwoLists) {
    ASSERT_EQ(parse("(1 2 3 4)"),
              eval(parse("(append (quote (1 2)) (quote (3 4)))")));
}

TEST(Append, ReturnsSingleAtomArgIntact) {
    ASSERT_EQ(1, intValue(eval(parse("(append 1)"))));
}

TEST(Append, CannotAppendAtomToCons) {
    ASSERT_THROW(eval(parse("(append 1 (quote ()))")), scheme_error);
}

TEST(Append, CannotAppendImproperListToList) {
    ASSERT_THROW(eval(parse("(append (cons 1 2) (quote ()))")), scheme_error);
}

TEST(Append, AppendsProperListToImproperList) {
    ASSERT_NO_THROW(eval(parse("(append (quote (1)) (cons 2 3))")));
    ASSERT_EQ(3, intValue(
                  eval(parse("(cdr (cdr (append (quote (1)) (cons 2 3)))))"))));
}

TEST(Append, AppendsMultipleLists) {
    auto program = "(append (quote (1)) (quote (2)) (quote (3)))";
    ASSERT_EQ(parse("(1 2 3)"), eval(parse(program)));
}

// car

TEST(Car, ThrowsOnEmptyList) {
    ASSERT_THROW(eval(parse("(car (quote ()))")), scheme_error);
}

TEST(Car, ThrowsOnNonConsArg) {
    ASSERT_THROW(eval(parse("(car 3)")), scheme_error);
}

TEST(Car, ThrowsWithMoreThanOneArg) {
    ASSERT_THROW(eval(parse("(car (quote (1)) (quote (2)))")), scheme_error);
}

TEST(Car, ReturnsFirstElementOfList) {
    ASSERT_EQ(1, intValue(eval(parse("(car (quote (1)))"))));
}

// cdr

TEST(Cdr, ThrowsWithNoArgs) {
    ASSERT_THROW(eval(parse("(cdr)")), scheme_error);
}

TEST(Cdr, ThrowsWithMoreThanOneArg) {
    ASSERT_THROW(eval(parse("(cdr (quote (1)) (quote (2)))")), scheme_error);
}

TEST(Cdr, ThrowsWithNonConsArg) {
    ASSERT_THROW(eval(parse("(cdr 1)")), scheme_error);
}

TEST(Cdr, RetrievesCdrFromCons) {
    ASSERT_EQ(2, intValue(eval(parse("(cdr (cons 1 2))"))));
}

// cons

// Retrieving the car and cdr is tested under car and cdr

TEST(Cons, ThrowsWithNoArgs) {
    ASSERT_THROW(eval(parse("(cons)")), scheme_error);
}

TEST(Cons, ThrowsWithOneArg) {
    ASSERT_THROW(eval(parse("(cons 1)")), scheme_error);
}

TEST(Cons, ThrowsWithMoreThanTwoArgs) {
    ASSERT_THROW(eval(parse("(cons 1 2 3)")), scheme_error);
}

// cons?

TEST(ConsP, ThrowsWithNoArgs) {
    ASSERT_THROW(eval(parse("(cons?)")), scheme_error);
}

TEST(ConsP, ThrowsWithMoreThanOneArg) {
    ASSERT_THROW(eval(parse("(cons? 1 2)")), scheme_error);
}

TEST(ConsP, ReturnsTrueWithConsArg) {
    ASSERT_TRUE(boolValue(eval(parse("(cons? (cons 1 2))"))));
}

TEST(ConsP, ReturnsFalseWithNonConsArg) {
    ASSERT_FALSE(boolValue(eval(parse("(cons? 1)"))));
}

TEST(ConsP, ReturnsFalseWithNil) {
    ASSERT_FALSE(boolValue(eval(parse("(cons? (quote ()))"))));
}

// eq?

TEST(Eq, ThrowsWithLessThanTwoArgs) {
    ASSERT_THROW(eval(parse("(eq?)")), scheme_error);
    ASSERT_THROW(eval(parse("(eq? (quote foo))")), scheme_error);
}

TEST(Eq, ThrowsWithMoreThanTwoArgs) {
    ASSERT_THROW(eval(parse("(eq? (quote foo) (quote bar) (quote baz))")),
                 scheme_error);
}

TEST(Eq, ThrowsWithNonSymbolArgs) {
    ASSERT_THROW(eval(parse("(eq? 2 2)")), scheme_error);
}

TEST(Eq, ReturnsTrueWithIdenticalSymbolArgs) {
    ASSERT_TRUE(boolValue(eval(parse("(eq? (quote foo) (quote foo))"))));
}

TEST(Eq, ReturnsFalseWithDistinctSymbolArgs) {
    ASSERT_FALSE(boolValue(eval(parse("(eq? (quote foo) (quote bar))"))));
}

// equal?

TEST(Equal, ThrowsWithNoArgs) {
    ASSERT_THROW(eval(parse("(equal?)")), scheme_error);
}

TEST(Equal, ThrowsWithOneArg) {
    ASSERT_THROW(eval(parse("(equal? #t)")), scheme_error);
}

TEST(Equal, ThrowsWithMoreThanTwoArgs) {
    ASSERT_THROW(eval(parse("(equal? #t #t #t)")), scheme_error);
}

TEST(Equal, AllowsComparisonsWithBools) {
    ASSERT_TRUE(boolValue(eval(parse("(equal? #t #t)"))));
    ASSERT_FALSE(boolValue(eval(parse("(equal? #t #f)"))));
}

TEST(Equal, AllowsComparisonsWithStrings) {
    ASSERT_TRUE(boolValue(eval(parse("(equal? \"um\" \"um\")"))));
    ASSERT_FALSE(boolValue(eval(parse("(equal? \"um\" \"hi\")"))));
}

TEST(Equal, AllowsComparisonsWithConses) {
    ASSERT_TRUE(boolValue(eval(parse("(equal? (cons 1 (quote ()))\
                                              (cons 1 (quote ())))"))));
    ASSERT_FALSE(boolValue(eval(parse("(equal? (cons 1 (quote ()))\
                                               (cons 2 (quote ())))"))));
    ASSERT_TRUE(boolValue(eval(parse("(equal? (cons 1 (cons 2 (quote ())))\
                                              (cons 1 (cons 2 (quote ()))))"
                                   ))));
}

TEST(Equal, AllowsComparisonsWithNil) {
    ASSERT_FALSE(boolValue(eval(parse("(equal? (quote (1 2 3)) (quote ()))"))));
}

TEST(Equal, ConsidersEqualCharsEqual) {
    ASSERT_TRUE(boolValue(eval(parse("(equal? #\\a #\\a)"))));
}

TEST(Equal, DoesCaseSensitiveComparisons) {
    ASSERT_FALSE(boolValue(eval(parse("(equal? 'foo 'FOO)"))));
    ASSERT_FALSE(boolValue(eval(parse("(equal? \"foo\" \"FOO\")"))));
    ASSERT_FALSE(boolValue(eval(parse("(equal? #\\x #\\X)"))));
}

// length

TEST(Length, ThrowsWithNoArgs) {
    ASSERT_THROW(eval(parse("(length)")), scheme_error);
}

TEST(Length, ThrowsWithMoreThanOneArg) {
    ASSERT_THROW(eval(parse("(length (quote ()) (quote ()))")), scheme_error);
}

TEST(Length, SaysNilIsOfLengthZero) {
    ASSERT_EQ(0, intValue(eval(parse("(length (quote ()))"))));
}

TEST(Length, ReturnsTheLengthOfAProperList) {
    ASSERT_EQ(3, intValue(eval(parse("(length (quote (1 2 3)))"))));
}

TEST(Length, ThrowsWithNonConsArg) {
    ASSERT_THROW(eval(parse("(length #t)")), scheme_error);
}

TEST(Length, ThrowsWithImproperListArg) {
    ASSERT_THROW(eval(parse("(length (cons 1 2))")), scheme_error);
}

// not

TEST(Not, ThrowsWithNoArgs) {
    ASSERT_THROW(eval(parse("(not)")), scheme_error);
}

TEST(Not, ThrowsWithMoreThanOneArg) {
    ASSERT_THROW(eval(parse("(not #t #t)")), scheme_error);
}

TEST(Not, ThrowsOnNonBoolArg) {
    ASSERT_THROW(eval(parse("(not 1)")), scheme_error);
    ASSERT_THROW(eval(parse("(not (quote um))")), scheme_error);
}

TEST(Not, ReturnsFalseOnTrue) {
    ASSERT_FALSE(boolValue(eval(parse("(not #t)"))));
}

TEST(Not, ReturnsTrueOnFalse) {
    ASSERT_TRUE(boolValue(eval(parse("(not #f)"))));
}

// null?

TEST(Null, ThrowsWithNoArgs) {
    ASSERT_THROW(eval(parse("(null?)")), scheme_error);
}

TEST(Null, ThrowsWithMoreThanOneArg) {
    ASSERT_THROW(eval(parse("(null? 1 2)")), scheme_error);
}

TEST(Null, ReturnsTrueOnTheEmptyList) {
    ASSERT_TRUE(boolValue(eval(parse("(null? (quote ()))"))));
}

TEST(Null, ReturnsFalseExceptOnTheEmptyList) {
    ASSERT_FALSE(boolValue(eval(parse("(null? 1)"))));
    ASSERT_FALSE(boolValue(eval(parse("(null? (cons 1 '()))"))));
    ASSERT_FALSE(boolValue(eval(parse("(null? #t)"))));
}

// string-length

TEST(StringLength, ThrowsOnNoArgs) {
    ASSERT_THROW(eval(parse("(string-length)")), scheme_error);
}

TEST(StringLength, ThrowsWithMoreThanOneArg) {
    ASSERT_THROW(eval(parse("(string-length \"um\" \"hi\")")), scheme_error);
}

TEST(StringLength, ThrowsWithNonStringArg) {
    ASSERT_THROW(eval(parse("(string-length (quote (1 2 3)))")), scheme_error);
}

TEST(StringLength, SaysTheEmptyStringIsOfZeroLength) {
    ASSERT_EQ(0, intValue(eval(parse("(string-length \"\")"))));
}

TEST(StringLength, ReturnsTheNumberOfCharactersInAString) {
    ASSERT_EQ(2, intValue(eval(parse("(string-length \"um\")"))));
}

TEST(StringLength, CountsNewlinesAsCharacters) {
    ASSERT_EQ(1, intValue(eval(parse("(string-length \"\n\")"))));
}

// string-ref

TEST(StringRef, ThrowsWithNoArgs) {
    ASSERT_THROW(eval(parse("(string-ref)")), scheme_error);
}

TEST(StringRef, ThrowsWithOneArg) {
    ASSERT_THROW(eval(parse("(string-ref \"um\")")), scheme_error);
}

TEST(StringRef, ThrowsWithMoreThanTwoArgs) {
    ASSERT_THROW(eval(parse("(string-ref \"um\" 1 2)")), scheme_error);
}

TEST(StringRef, ThrowsWhenFirstArgIsNotAString) {
    ASSERT_THROW(eval(parse("(string-ref (quote um) 1)")), scheme_error);
}

TEST(StringRef, ThrowsWhenSecondArgIsNotAnInteger) {
    ASSERT_THROW(eval(parse("(string-ref \"um\" #\\u)")), scheme_error);
}

TEST(StringRef, RetrievesCharacterWithValidStringAndIndex) {
    ASSERT_EQ('u', charValue(eval(parse("(string-ref \"um\" 0)"))));
}

// cons?

TEST(Consp, ThrowsWithNoArgs) {
    ASSERT_THROW(eval(parse("(cons?)")), scheme_error);
}

TEST(Consp, ThrowsWithMoreThanOneArg) {
    ASSERT_THROW(eval(parse("(cons? (cons 1 2) (cons 3 4))")), scheme_error);
}

TEST(Consp, ReturnsTrueWhenGivenACons) {
    ASSERT_TRUE(boolValue(eval(parse("(cons? (cons 1 2))"))));
}

TEST(Consp, ReturnsFalseWithNonConsArg) {
    ASSERT_FALSE(boolValue(eval(parse("(cons? (quote ()))"))));
    ASSERT_FALSE(boolValue(eval(parse("(cons? #t)"))));
    ASSERT_FALSE(boolValue(eval(parse("(cons? 1)"))));
    ASSERT_FALSE(boolValue(eval(parse("(cons? \"um\")"))));
}

// string?

TEST(Stringp, ThrowsWithNoArgs) {
    ASSERT_THROW(eval(parse("(string?)")), scheme_error);
}

TEST(Stringp, ThrowsWithMoreThanOneArg) {
    ASSERT_THROW(eval(parse("(string? \"um\" \"hi\")")), scheme_error);
}

TEST(Stringp, ReturnsTrueWithStringArg) {
    ASSERT_TRUE(boolValue(eval(parse("(string? \"um\")"))));
}

TEST(Stringp, ReturnsFalseWithNonStringArg) {
    ASSERT_FALSE(boolValue(eval(parse("(string? #t)"))));
    ASSERT_FALSE(boolValue(eval(parse("(string? 1)"))));
    ASSERT_FALSE(boolValue(eval(parse("(string? #\\a)"))));
    ASSERT_FALSE(boolValue(eval(parse("(string? (cons 1 2))"))));
}

// number?

TEST(Numberp, ThrowsWithNoArgs) {
    ASSERT_THROW(eval(parse("(number?)")), scheme_error);
}

TEST(Numberp, ThrowsWithMoreThanOneArg) {
    ASSERT_THROW(eval(parse("(number? 1 2)")), scheme_error);
}

TEST(Numberp, ReturnsTrueWithNumericArg) {
    ASSERT_TRUE(boolValue(eval(parse("(number? 1)"))));
}

TEST(Numberp, ReturnsFalseWithNonNumericArg) {
    ASSERT_FALSE(boolValue(eval(parse("(number? #t)"))));
    ASSERT_FALSE(boolValue(eval(parse("(number? \"um\")"))));
    ASSERT_FALSE(boolValue(eval(parse("(number? #\\a)"))));
    ASSERT_FALSE(boolValue(eval(parse("(number? (cons 1 2))"))));
}

// symbol?

TEST(Symbolp, ThrowsWithNoArgs) {
    ASSERT_THROW(eval(parse("(symbol?)")), scheme_error);
}

TEST(Symbolp, ThrowsWithMoreThanOneArg) {
    ASSERT_THROW(eval(parse("(symbol? (quote a) (quote b))")), scheme_error);
}

TEST(Symbolp, ReturnsTrueWithSymbolArg) {
    ASSERT_TRUE(boolValue(eval(parse("(symbol? (quote foo))"))));
}

TEST(Symbolp, ReturnsFalseWithNonSymbolArg) {
    ASSERT_FALSE(boolValue(eval(parse("(symbol? #t)"))));
    ASSERT_FALSE(boolValue(eval(parse("(symbol? \"um\")"))));
    ASSERT_FALSE(boolValue(eval(parse("(symbol? #\\a)"))));
    ASSERT_FALSE(boolValue(eval(parse("(symbol? (cons 1 2))"))));
    ASSERT_FALSE(boolValue(eval(parse("(symbol? (quote ()))"))));
}

// character?

TEST(Characterp, ThrowsWithNoArgs) {
    ASSERT_THROW(eval(parse("(character?)")), scheme_error);
}

TEST(Characterp, ThrowsWithMoreThanOneArg) {
    ASSERT_THROW(eval(parse("(character? #\\a #\\b)")), scheme_error);
}

TEST(Characterp, ReturnsTrueWithCharacterArg) {
    ASSERT_TRUE(boolValue(eval(parse("(character? #\\a)"))));
}

TEST(Characterp, ReturnsFalseWithNonCharacterArg) {
    ASSERT_FALSE(boolValue(eval(parse("(character? #t)"))));
    ASSERT_FALSE(boolValue(eval(parse("(character? \"um\")"))));
    ASSERT_FALSE(boolValue(eval(parse("(character? (cons 1 2))"))));
    ASSERT_FALSE(boolValue(eval(parse("(character? (quote ()))"))));
}
