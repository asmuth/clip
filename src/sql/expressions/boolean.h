/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_SQL_EXPRESSIONS_BOOLEAN_H
#define _FNORDMETRIC_SQL_EXPRESSIONS_BOOLEAN_H
#include <fnordmetric/sql/svalue.h>

namespace fnordmetric {
namespace query {
namespace expressions {

void eqExpr(void* scratchpad, int argc, SValue* argv, SValue* out);
void neqExpr(void* scratchpad, int argc, SValue* argv, SValue* out);
void andExpr(void* scratchpad, int argc, SValue* argv, SValue* out);
void orExpr(void* scratchpad, int argc, SValue* argv, SValue* out);
void negExpr(void* scratchpad, int argc, SValue* argv, SValue* out);
void ltExpr(void* scratchpad, int argc, SValue* argv, SValue* out);
void lteExpr(void* scratchpad, int argc, SValue* argv, SValue* out);
void gtExpr(void* scratchpad, int argc, SValue* argv, SValue* out);
void gteExpr(void* scratchpad, int argc, SValue* argv, SValue* out);

}
}
}
#endif
