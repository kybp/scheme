# scheme

Simple C++11 Scheme interpreter based off Peter Norvig's
[lis.py](http://norvig.com/lispy.html). For datatypes, only fixnums,
symbols, functions, bools, and conses have been implemented. For
special forms, there is begin, define, set!, lambda, if, and, or, and
quote. For primitive functions, there is +, -, *, abs, eq?, car, cdr,
cons, cons?, length, not, null?, <, >, and =. Additionally, functions
can take a rest parameter by prefixing it with '&rest' in a lambda
list.

## Dependencies

Requires Boost.Variant
