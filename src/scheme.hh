#ifndef SCHEME_HH
#define SCHEME_HH

#include <boost/variant.hpp>

std::vector<std::string> tokenize(const std::string string);
boost::variant<int, std::string> readToken(const std::string token);

#endif
