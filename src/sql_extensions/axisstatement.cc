/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <stdlib.h>
#include <fnordmetric/sql_extensions/axisstatement.h>
#include <fnordmetric/ui/drawable.h>

namespace fnordmetric {
namespace query {

AxisStatement::AxisStatement(ASTNode* ast) {}

void AxisStatement::execute(ui::Drawable* drawable) const {
  drawable->addAxis(ui::AxisDefinition::LEFT); // FIPXAUL
  drawable->addAxis(ui::AxisDefinition::BOTTOM); // FIPXAUL
  drawable->addAxis(ui::AxisDefinition::TOP); // FIPXAUL
  drawable->addAxis(ui::AxisDefinition::RIGHT); // FIPXAUL
}

}
}
