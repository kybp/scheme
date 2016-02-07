#ifndef SCHEME_HH
#define SCHEME_HH

#include <deque>
#include <string>
#include <boost/variant.hpp>

typedef boost::make_recursive_variant<
    int, std::string, std::deque<boost::recursive_variant_>
    >::type SchemeExpr;

std::deque<std::string> tokenize(const std::string string);
SchemeExpr parse(const std::string& program);

#endif
