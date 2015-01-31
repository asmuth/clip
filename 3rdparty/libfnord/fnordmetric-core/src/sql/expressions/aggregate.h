/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_SQL_EXPRESSIONS_AGGREGATE_H
#define _FNORDMETRIC_SQL_EXPRESSIONS_AGGREGATE_H
#include <sql/svalue.h>

namespace fnordmetric {
namespace query {
namespace expressions {

void countExpr(void* scratchpad, int argc, SValue* argv, SValue* out);
void countExprFree(void* scratchpad);
size_t countExprScratchpadSize();

void sumExpr(void* scratchpad, int argc, SValue* argv, SValue* out);
void sumExprFree(void* scratchpad);
size_t sumExprScratchpadSize();

void meanExpr(void* scratchpad, int argc, SValue* argv, SValue* out);
void meanExprFree(void* scratchpad);
size_t meanExprScratchpadSize();

void minExpr(void* scratchpad, int argc, SValue* argv, SValue* out);
void minExprFree(void* scratchpad);
size_t minExprScratchpadSize();

void maxExpr(void* scratchpad, int argc, SValue* argv, SValue* out);
void maxExprFree(void* scratchpad);
size_t maxExprScratchpadSize();

}
}
}
#endif
