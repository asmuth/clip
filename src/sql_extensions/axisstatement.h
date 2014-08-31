/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_QUERY_AXISSTATEMENT_H
#define _FNORDMETRIC_QUERY_AXISSTATEMENT_H
#include <stdlib.h>
#include <assert.h>
#include <fnordmetric/sql/compile.h>
#include <fnordmetric/sql/execute.h>
#include <fnordmetric/sql/queryplannode.h>
#include <fnordmetric/ui/axisdefinition.h>

namespace fnordmetric {
namespace ui {
class Drawable;
}

namespace query {

class AxisStatement {
public:
  AxisStatement(ASTNode* ast);
  void execute(ui::Drawable* drawable) const;
};

}
}
#endif
