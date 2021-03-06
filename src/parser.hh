#ifndef PARSER_HH
#define PARSER_HH

#include <deque>
#include <string>
#include "printer.hh"
#include "scheme_types.hh"

SchemeExpr parse(const std::string& expression);
std::istream& readToken(std::istream& in, std::string& out);
std::istream& readSchemeExpr(std::istream& in, SchemeExpr& out);

inline char charValue(const SchemeExpr& e)
{
    try {
        return boost::get<char>(e);
    } catch (const boost::bad_get&){
        std::ostringstream error;
        error << "Character expected, got " << e;
        throw scheme_error(error);
    }
}

inline int intValue(const SchemeExpr& e)
{
    try {
        return boost::get<int>(e);
    } catch (const boost::bad_get&) {
        std::ostringstream error;
        error << "Integer expected, got " << e;
        throw scheme_error(error);
    }
}

inline std::string stringValue(const SchemeExpr& e)
{
    try {
        return boost::get<std::string>(e);
    } catch (const boost::bad_get&) {
        std::ostringstream error;
        error << "String expected, got " << e;
        throw scheme_error(error);
    }
}

inline SchemeSymbol symbolValue(const SchemeExpr& e)
{
    try {
        return boost::get<SchemeSymbol>(e);
    } catch (const boost::bad_get&) {
        std::ostringstream error;
        error << "Symbol expected, got " << e;
        throw scheme_error(error);
    }
}

inline bool boolValue(const SchemeExpr& e)
{
    try {
        return boost::get<bool>(e);
    } catch (const boost::bad_get&) {
        std::ostringstream error;
        error << "Boolean expected, got " << e;
        throw scheme_error(error);
    }
}

inline std::shared_ptr<SchemeFunction> functionPointer(const SchemeExpr& e)
{
    try {
        return boost::get<std::shared_ptr<SchemeFunction>>(e);
    } catch (const boost::bad_get&) {
        std::ostringstream error;
        error << "Function expected, got " << e;
        throw scheme_error(error);
    }
}

// consValue is in scheme_types.hh because it's needed for converting
// a cons to a std::vector

#endif
