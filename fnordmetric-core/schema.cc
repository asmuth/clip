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

IField::IField(schema::kFieldType type, const std::string& name) :
      type_(type),
      name_(name) {}

const std::string& IField::getName() const {
  return name_;
}

schema::kFieldType IField::getTypeId() const {
  return type_;
}

ISchema::ISchema(const std::vector<IField>& fields) : fields_(fields) {}

const std::vector<IField>& ISchema::getFields() const {
  return fields_;
}


}
