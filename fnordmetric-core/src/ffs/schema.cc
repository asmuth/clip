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
#include <stdint.h>
#include "record.h"

namespace fnordmetric {

Schema::Schema(const std::vector<Field>& fields) : fields_(fields) {}

Field::Field(schema::kFieldType type, const std::string& name) :
      type_(type),
      name_(name) {}

const std::string& Field::getName() const {
  return name_;
}

schema::kFieldType Field::getTypeId() const {
  return type_;
}

}
