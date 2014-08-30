/**
 * this file is part of the "fnordmetric" project
 *   copyright (c) 2011-2014 paul asmuth, google inc.
 *
 * fnordmetric is free software: you can redistribute it and/or modify it under
 * the terms of the gnu general public license v3.0. you should have received a
 * copy of the gnu general public license along with this program. if not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <stdlib.h>
#include <fnordmetric/sql_extensions/axisstatement.h>
#include <fnordmetric/ui/drawable.h>

namespace fnordmetric {
namespace query {

void AxisStatement::execute(ui::Drawable* drawable) {
  drawable->addAxis(ui::AxisDefinition::LEFT); // FIPXAUL
  drawable->addAxis(ui::AxisDefinition::BOTTOM); // FIPXAUL
  drawable->addAxis(ui::AxisDefinition::TOP); // FIPXAUL
  drawable->addAxis(ui::AxisDefinition::RIGHT); // FIPXAUL
}

}
}
