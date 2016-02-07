#ifndef BUILTINS_HH
#define BUILTINS_HH

#include "scheme_types.hh"

SchemeEnvironment standardEnvironment();
SchemeExpr eval(SchemeExpr e); // evaluate in standard environment

#endif
