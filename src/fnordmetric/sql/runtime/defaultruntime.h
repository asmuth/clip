/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_SQL_DEFAULTRUNTIME_H
#define _FNORDMETRIC_SQL_DEFAULTRUNTIME_H
#include <fnordmetric/sql/runtime/runtime.h>

namespace fnordmetric {
namespace query {

class DefaultRuntime : public Runtime {
public:
  DefaultRuntime();
};

}
}
#endif
