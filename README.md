# scheme

Simple C++11 Scheme interpreter based off Peter Norvig's
[lis.py](http://norvig.com/lispy.html).

## Dependencies

Requires Boost

## Types and operations

### Special operators

* define assigns a symbol to a value in the current environment.
  
* set! replaces the visible definition of a symbol.

* begin takes any number of forms and evaluates them in turn,
  returning the result of the last form.

* lambda returns a function. it takes a list of parameters to bind to
  its arguments during execution of its body when it is called. There
  is an implicit begin around the body forms of a lambda. The
  parameter list can also have a single rest parameter following the
  token "&rest" which will be bound to any number of arguments, eg
  (define number-of-args (lambda (&rest args) (length args)))

* if takes a boolean and two forms; if the boolean evaluates to true
  it evaluates and returns the value of the first form, otherwise it
  evaluates and returns the value of the second form.

* or takes any number of arguments and evaluates them in turn,
  stopping and returning the first non-#f value, or #f if no such
  value is found.

* and takes any number of arguments and evaluates them in turn,
  stopping at the first #f value. If no argument evaluates to #f, it
  returns the last argument it evaluated. (and) evaluates to #t.

* quote takes one argument and returns it unevaluated.

### General Functions

* equal? returns #t if its arguments are equal, and #f otherwise
* display prints its argument
* newline prints a newline; equivalent to (display "\n")

### Booleans

The two boolean values are #t, indicating truth, and #f, indicating
falsehood.

* (not #t) returns #f, (not #f) returns #t

### Numbers

Only integers are supported. They consist of possibly a minus sign,
followed by any number of digits.

* arithmetic functions: +, -, *
* <, =, and > compare two numbers and return a boolean
* abs returns the absolute value of its argument
* number? returns #t if its argument is a number, and #f otherwise

### Symbols

Symbols are sequences of characters that cannot be read as an integer,
and do not contain double quotes, whitespace, or open or close
parentheses.

* eq? compares two symbols for equality
* symbol? returns #t if its argument is a symbol, and #f otherwise

### Lists

A cons is a pair of two values, called its car and its cdr. If a
cons's cdr is either a cons or the empty list, the cons is a proper
list. The empty list, written (), is not a cons, but serves to mark
the end of a chain of conses representing a list.

* car returns the car of a cons
* cdr returns the cdr of a cons
* cons takes two arguments, the first representing the car and the
  second representing the cdr, and constructs a cons containing them
* cons? returns #t if its argument is a cons, or #f otherwise
* length returns the length of a proper list
* null? returns #t if its argument is the empty list, or #f otherwise

### Strings

Strings are any number of characters inbetween two " characters. A "
can be embedded in a string by prefixing it with a backslash. Newlines
and tabs can be included literally in strings, or written as \n or \t
respectively. A literal backslash can be included by doubling it.

* string? returns #t if its argument is a string, and #f otherwise
* string-length returns the length of a string
* string-ref takes a string and an integer representing a 0-based
  index into the string, and returns the corresponding character

### Characters

Characters are writen as a hash followed by a backslash and a
character name. Characters may be written literally (eg, #\a), but for
whitespace characters this can be confusing, so space, newline, and
tab can be written using (case-insensitive) names (eg, #\space).

* character? returns #t if its argument is a character, and #f
  otherwise
