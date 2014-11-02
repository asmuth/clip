/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_SQL_EXPRESSIONS_MATH_H
#define _FNORDMETRIC_SQL_EXPRESSIONS_MATH_H
#include <fnordmetric/sql/svalue.h>

namespace fnordmetric {
namespace query {
namespace expressions {

void addExpr(void* scratchpad, int argc, SValue* argv, SValue* out);
void subExpr(void* scratchpad, int argc, SValue* argv, SValue* out);
void mulExpr(void* scratchpad, int argc, SValue* argv, SValue* out);
void divExpr(void* scratchpad, int argc, SValue* argv, SValue* out);
void modExpr(void* scratchpad, int argc, SValue* argv, SValue* out);
void powExpr(void* scratchpad, int argc, SValue* argv, SValue* out);

}
}
}
#endif
