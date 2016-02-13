#include "gtest/gtest.h"
#include "builtins.hh"
#include "parser.hh"
#include "scheme_types.hh"

// +

TEST(AdditionTests, BasicAddition) {
    ASSERT_EQ(3, intValue(eval(parse("(+ 1 2)"))));
}

TEST(AdditionTests, Identity) {
    ASSERT_EQ(0, intValue(eval(parse("(+)"))));
}

TEST(AdditionTests, NonIntegralThrows) {
    ASSERT_THROW(eval(parse("(+ 1 (quote foo) 2)")), scheme_error);
}

TEST(AdditionTests, ArgumentsAreEvaluated) {
    ASSERT_EQ(6, intValue(eval(parse("(+ (+ 1 2) 3)"))));
}

// -

TEST(SubtractionTests, NoArgsThrows) {
    ASSERT_THROW(eval(parse("(-)")), scheme_error);
}

TEST(SubtractionTests, OneArgNegates) {
    ASSERT_EQ(-1, intValue(eval(parse("(- 1)"))));
}

TEST(SubtractionTests, BasicSubtraction) {
    ASSERT_EQ(1, intValue(eval(parse("(- 3 2)"))));
}

// *

TEST(MultiplicationTest, Identity) {
    ASSERT_EQ(1, intValue(eval(parse("(*)"))));
}

TEST(MultiplicationTest, BasicMultiplication) {
    ASSERT_EQ(6, intValue(eval(parse("(* 1 2 3)"))));
}

// <

TEST(LessThanTest, LessThanTwoArgsThrows) {
    ASSERT_THROW(eval(parse("(<)")), scheme_error);
    ASSERT_THROW(eval(parse("(< 1)")), scheme_error);
}

TEST(LessThanTest, LessThanIsTrue) {
    ASSERT_TRUE(boolValue(eval(parse("(< 1 2)"))));
}

TEST(LessThanIsTrue, EqualIsFalse) {
    ASSERT_FALSE(boolValue(eval(parse("(< 2 2)"))));
}

TEST(LessThanTest, GreaterThanIsFalse) {
    ASSERT_FALSE(boolValue(eval(parse("(< 2 1)"))));
}

TEST(LessThanTest, NonIntegerThrows) {
    ASSERT_THROW(eval(parse("(< (quote a) (quote b))")), scheme_error);
}

// =

TEST(EqualOpTest, LessThanTwoArgsThrows) {
    ASSERT_THROW(eval(parse("(=)")), scheme_error);
    ASSERT_THROW(eval(parse("(= 1)")), scheme_error);
}

TEST(EqualOpTest, NonIntegerThrows) {
    ASSERT_THROW(eval(parse("(= 2 (quote um))")), scheme_error);
}

TEST(EqualOpTest, SameIntegerIsTrue) {
    ASSERT_TRUE(boolValue(eval(parse("(= 2 2)"))));
}

TEST(EqualOpTest, DifferentIntegersIsFalse) {
    ASSERT_FALSE(boolValue(eval(parse("(= 2 2 3)"))));
}

// >

TEST(GreaterTest, LessThanTwoArgsThrows) {
    ASSERT_THROW(eval(parse("(>)")), scheme_error);
    ASSERT_THROW(eval(parse("(> 1)")), scheme_error);
}

TEST(GreaterTest, LessThanIsFalse) {
    ASSERT_FALSE(boolValue(eval(parse("(> 1 2)"))));
}

TEST(GreaterTest, EqualIsFalse) {
    ASSERT_FALSE(boolValue(eval(parse("(> 2 2)"))));
}

TEST(GreaterTest, GreaterIsTrue) {
    ASSERT_TRUE(boolValue(eval(parse("(> 3 2 1)"))));
}

// abs

TEST(AbsTests, PositiveInput) {
    ASSERT_EQ(1, intValue(eval(parse("(abs 1)"))));
}

TEST(AbsTests, NegativeInput) {
    ASSERT_EQ(1, intValue(eval(parse("(abs -1)"))));
}

// car

TEST(CarTests, CarOfNilThrows) {
    ASSERT_THROW(eval(parse("(car (quote ()))")), scheme_error);
}

TEST(CarTests, NonConsArgThrows) {
    ASSERT_THROW(eval(parse("(car 3)")), scheme_error);
}

TEST(CarTests, MoreThanOneArgThrows) {
    ASSERT_THROW(eval(parse("(car (quote (1)) (quote (2)))")), scheme_error);
}

TEST(CarTests, CarReturnsFirstExpr) {
    ASSERT_EQ(1, intValue(eval(parse("(car (quote (1)))"))));
}

// cdr

TEST(CdrTests, NoArgsThrows) {
    ASSERT_THROW(eval(parse("(cdr)")), scheme_error);
}

TEST(CdrTests, MoreThanOneArgThrows) {
    ASSERT_THROW(eval(parse("(cdr (quote (1)) (quote (2)))")), scheme_error);
}

TEST(CdrTests, NonConsArgThrows) {
    ASSERT_THROW(eval(parse("(cdr 1)")), scheme_error);
}

TEST(CdrTests, RetrievesCdrFromCons) {
    ASSERT_EQ(2, intValue(eval(parse("(cdr (cons 1 2))"))));
}

// cons

TEST(ConsTests, NoArgsThrows) {
    ASSERT_THROW(eval(parse("(cons)")), scheme_error);
}

TEST(ConsTests, OneArgThrows) {
    ASSERT_THROW(eval(parse("(cons 1)")), scheme_error);
}

TEST(ConsTests, MoreThanOneArgThrows) {
    ASSERT_THROW(eval(parse("(cons 1 2 3)")), scheme_error);
}

TEST(ConsTests, RetrievableWithCar) {
    ASSERT_EQ(1, intValue(eval(parse("(car (cons 1 2))"))));
}

// cons?

TEST(ConsPTests, NoArgsThrows) {
    ASSERT_THROW(eval(parse("(cons?)")), scheme_error);
}

TEST(ConsPTests, MoreThanOneArgThrows) {
    ASSERT_THROW(eval(parse("(cons? 1 2)")), scheme_error);
}

TEST(ConsPTests, ConsIsACons) {
    ASSERT_TRUE(boolValue(eval(parse("(cons? (cons 1 2))"))));
}

TEST(ConsPTests, NotConsIsNotACons) {
    ASSERT_FALSE(boolValue(eval(parse("(cons? 1)"))));
}

TEST(ConsPTests, NilIsNotACons) {
    ASSERT_FALSE(boolValue(eval(parse("(cons? (quote ()))"))));
}

// eq?

TEST(EqTests, LessThanTwoArgsThrows) {
    ASSERT_THROW(eval(parse("(eq?)")), scheme_error);
    ASSERT_THROW(eval(parse("(eq? (quote foo))")), scheme_error);
}

TEST(EqTests, MoreThanTwoArgsThrows) {
    ASSERT_THROW(eval(parse("(eq? (quote foo) (quote bar) (quote baz))")),
                 scheme_error);
}

TEST(EqTests, NonSymbolArgThrows) {
    ASSERT_THROW(eval(parse("(eq? 2 2)")), scheme_error);
}

TEST(EqTests, EqualSymbolsIsTrue) {
    ASSERT_TRUE(boolValue(eval(parse("(eq? (quote foo) (quote foo))"))));
}

TEST(EqTests, UnequalSymbolsIsFalse) {
    ASSERT_FALSE(boolValue(eval(parse("(eq? (quote foo) (quote bar))"))));
}

// equal?

TEST(EqualTests, NoArgsThrows) {
    ASSERT_THROW(eval(parse("(equal?)")), scheme_error);
}

TEST(EqualTests, OneArgThrows) {
    ASSERT_THROW(eval(parse("(equal? #t)")), scheme_error);
}

TEST(EqualTests, MoreThanTwoArgsThrows) {
    ASSERT_THROW(eval(parse("(equal? #t #t #t)")), scheme_error);
}

TEST(EqualTests, CompareBools) {
    ASSERT_TRUE(boolValue(eval(parse("(equal? #t #t)"))));
    ASSERT_FALSE(boolValue(eval(parse("(equal? #t #f)"))));
}

TEST(EqualTests, CompareStrings) {
    ASSERT_TRUE(boolValue(eval(parse("(equal? \"um\" \"um\")"))));
    ASSERT_FALSE(boolValue(eval(parse("(equal? \"um\" \"hi\")"))));
}

TEST(EqualTests, CompareLists) {
    ASSERT_TRUE(boolValue(eval(parse("(equal? (cons 1 (quote ()))\
                                              (cons 1 (quote ())))"))));
    ASSERT_FALSE(boolValue(eval(parse("(equal? (cons 1 (quote ()))\
                                               (cons 2 (quote ())))"))));
    ASSERT_TRUE(boolValue(eval(parse("(equal? (cons 1 (cons 2 (quote ())))\
                                              (cons 1 (cons 2 (quote ()))))"
                                   ))));
}

TEST(EqualTests, CompareListWithNil) {
    ASSERT_FALSE(boolValue(eval(parse("(equal? (quote (1 2 3)) (quote ()))"))));
}

// length

TEST(LengthTests, NoArgsThrows) {
    ASSERT_THROW(eval(parse("(length)")), scheme_error);
}

TEST(LengthTests, MoreThanOneArgThrows) {
    ASSERT_THROW(eval(parse("(length (quote ()) (quote ()))")), scheme_error);
}

TEST(LengthTests, EmptyListIsZeroLength) {
    ASSERT_EQ(0, intValue(eval(parse("(length (quote ()))"))));
}

TEST(LengthTests, ThreeElementListIsThree) {
    ASSERT_EQ(3, intValue(eval(parse("(length (quote (1 2 3)))"))));
}

TEST(LengthTests, NonListArgThrows) {
    ASSERT_THROW(eval(parse("(length #t)")), scheme_error);
}

TEST(LengthTests, ImproperListArgThrows) {
    ASSERT_THROW(eval(parse("(length (cons 1 2))")), scheme_error);
}

// not

TEST(NotTests, NoArgsThrows) {
    ASSERT_THROW(eval(parse("(not)")), scheme_error);
}

TEST(NotTests, MoreThanOneArgThrows) {
    ASSERT_THROW(eval(parse("(not #t #t)")), scheme_error);
}

TEST(NotTests, NonBoolArgThrows) {
    ASSERT_THROW(eval(parse("(not 1)")), scheme_error);
    ASSERT_THROW(eval(parse("(not (quote um))")), scheme_error);
}

TEST(NotTests, NotTrueIsFalse) {
    ASSERT_FALSE(boolValue(eval(parse("(not #t)"))));
}

TEST(NotTests, NotFalseIsTrue) {
    ASSERT_TRUE(boolValue(eval(parse("(not #f)"))));
}

// null?

TEST(NullTests, NoArgsThrows) {
    ASSERT_THROW(eval(parse("(null?)")), scheme_error);
}

TEST(NullTests, MoreThanOneArgThrows) {
    ASSERT_THROW(eval(parse("(null? 1 2)")), scheme_error);
}

TEST(NullTests, NilIsNull) {
    ASSERT_TRUE(boolValue(eval(parse("(null? (quote ()))"))));
}

TEST(NullTests, NotNilIsFalse) {
    ASSERT_FALSE(boolValue(eval(parse("(null? 1)"))));
}

// string-length

TEST(StringLengthTests, NoArgsThrows) {
    ASSERT_THROW(eval(parse("(string-length)")), scheme_error);
}

TEST(StringLengthTests, MoreThanOneArgsThrows) {
    ASSERT_THROW(eval(parse("(string-length \"um\" \"hi\")")), scheme_error);
}

TEST(StringLengthTests, NonStringArgThrows) {
    ASSERT_THROW(eval(parse("(string-length (quote (1 2 3)))")), scheme_error);
}

TEST(StringLengthTests, EmptyStringIsZeroLength) {
    ASSERT_EQ(0, intValue(eval(parse("(string-length \"\")"))));
}

TEST(StringLengthTests, StringLength) {
    ASSERT_EQ(2, intValue(eval(parse("(string-length \"um\")"))));
}

// string-ref

TEST(StringRefTests, NoArgsThrows) {
    ASSERT_THROW(eval(parse("(string-ref)")), scheme_error);
}

TEST(StringRefTests, OneArgThrows) {
    ASSERT_THROW(eval(parse("(string-ref \"um\")")), scheme_error);
}

TEST(StringRefTests, MoreThanTwoArgsThrows) {
    ASSERT_THROW(eval(parse("(string-ref \"um\" 1 2)")), scheme_error);
}

TEST(StringRefTests, NonStringFirstArgThrows) {
    ASSERT_THROW(eval(parse("(string-ref (quote um) 1)")), scheme_error);
}

TEST(StringRefTests, NonIntegerSecondArgThrows) {
    ASSERT_THROW(eval(parse("(string-ref \"um\" #\\u)")), scheme_error);
}

TEST(StringRefTests, ValidIndexGetsCharacter) {
    ASSERT_EQ('u', charValue(eval(parse("(string-ref \"um\" 0)"))));
}

TEST(ConspTests, NoArgsThrows) {
    ASSERT_THROW(eval(parse("(cons?)")), scheme_error);
}

TEST(ConspTests, MoreThanOneArgThrows) {
    ASSERT_THROW(eval(parse("(cons? (cons 1 2) (cons 3 4))")), scheme_error);
}

TEST(ConspTests, ConsIsTrue) {
    ASSERT_TRUE(boolValue(eval(parse("(cons? (cons 1 2))"))));
}

TEST(ConspTests, NonConsIsFalse) {
    ASSERT_FALSE(boolValue(eval(parse("(cons? (quote ()))"))));
    ASSERT_FALSE(boolValue(eval(parse("(cons? #t)"))));
    ASSERT_FALSE(boolValue(eval(parse("(cons? 1)"))));
    ASSERT_FALSE(boolValue(eval(parse("(cons? \"um\")"))));
}

TEST(StringpTests, NoArgsThrows) {
    ASSERT_THROW(eval(parse("(string?)")), scheme_error);
}

TEST(StringpTests, MoreThanOneArgThrows) {
    ASSERT_THROW(eval(parse("(string? \"um\" \"hi\")")), scheme_error);
}

TEST(StringpTests, StringIsTrue) {
    ASSERT_TRUE(boolValue(eval(parse("(string? \"um\")"))));
}

TEST(StringpTests, NonStringIsFalse) {
    ASSERT_FALSE(boolValue(eval(parse("(string? #t)"))));
    ASSERT_FALSE(boolValue(eval(parse("(string? 1)"))));
    ASSERT_FALSE(boolValue(eval(parse("(string? #\\a)"))));
    ASSERT_FALSE(boolValue(eval(parse("(string? (cons 1 2))"))));
}

TEST(NumberpTests, NoArgsThrows) {
    ASSERT_THROW(eval(parse("(number?)")), scheme_error);
}

TEST(NumberpTests, MoreThanOneArgThrows) {
    ASSERT_THROW(eval(parse("(number? 1 2)")), scheme_error);
}

TEST(NumberpTests, NumberIsTrue) {
    ASSERT_TRUE(boolValue(eval(parse("(number? 1)"))));
}

TEST(NumberpTests, NonNumberIsFalse) {
    ASSERT_FALSE(boolValue(eval(parse("(number? #t)"))));
    ASSERT_FALSE(boolValue(eval(parse("(number? \"um\")"))));
    ASSERT_FALSE(boolValue(eval(parse("(number? #\\a)"))));
    ASSERT_FALSE(boolValue(eval(parse("(number? (cons 1 2))"))));
}

TEST(SymbolpTests, NoArgsThrows) {
    ASSERT_THROW(eval(parse("(symbol?)")), scheme_error);
}

TEST(SymbolpTests, MoreThanOneArgThrows) {
    ASSERT_THROW(eval(parse("(symbol? (quote a) (quote b))")), scheme_error);
}

TEST(SymbolpTests, SymbolIsTrue) {
    ASSERT_TRUE(boolValue(eval(parse("(symbol? (quote foo))"))));
}

TEST(SymbolpTests, NonSymbolIsFalse) {
    ASSERT_FALSE(boolValue(eval(parse("(symbol? #t)"))));
    ASSERT_FALSE(boolValue(eval(parse("(symbol? \"um\")"))));
    ASSERT_FALSE(boolValue(eval(parse("(symbol? #\\a)"))));
    ASSERT_FALSE(boolValue(eval(parse("(symbol? (cons 1 2))"))));
    ASSERT_FALSE(boolValue(eval(parse("(symbol? (quote ()))"))));
}
