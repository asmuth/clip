/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <stdint.h>
#include "record.h"

namespace fnordmetric {

Field::Field(schema::kFieldType type, const std::string& name) :
      type_(type),
      name_(name) {}

const std::string& Field::getName() const {
  return name_;
}

schema::kFieldType Field::getTypeId() const {
  return type_;
}

Schema::Schema(const std::vector<Field>& fields) : fields_(fields) {}

const std::vector<Field>& Schema::getFields() const {
  return fields_;
}


}
