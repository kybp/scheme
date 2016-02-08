#ifndef PARSER_HH
#define PARSER_HH

#include <deque>
#include <string>
#include "printer.hh"
#include "scheme_types.hh"

std::deque<std::string> tokenize(const std::string string);
SchemeExpr parse(const std::string& program);

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

inline SchemeList listValue(const SchemeExpr& e)
{
    try {
        return boost::get<std::deque<SchemeExpr>>(e);
    } catch (const boost::bad_get&) {
        std::ostringstream error;
        error << "List expected, got " << e;
        throw scheme_error(error);
    }
}

#endif
