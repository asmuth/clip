/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_SCHEMA_H
#define _FNORDMETRIC_SCHEMA_H

#include <stdlib.h>
#include <string>
#include <vector>

namespace fnordmetric {

namespace schema {
enum kFieldType {
  INT64  = 'I',
  IEE754 = 'F',
  STRING = 'S'
};
}

class Field {
public:
  const std::string& getName() const;
  schema::kFieldType getTypeId() const;
protected:
  Field(schema::kFieldType type, const std::string& name);
  schema::kFieldType type_; // FIXPAUL REMOVE ME
  std::string name_;
};

class Schema {
public:
  explicit Schema(const std::vector<Field>& fields);
  const std::vector<Field>& getFields() const;
protected:
  const std::vector<Field> fields_;
};

template <typename... T>
class TypedSchema : public Schema {
public:
  explicit TypedSchema(const T... fields);
protected:
  std::vector<Field> unpackFields(T... fields);
};

class IntegerField : public Field {
public:
  IntegerField(const std::string& name) : Field(schema::INT64, name) {}
  typedef int64_t ValueType;
};

class FloatField : public Field {
public:
  FloatField(const std::string& name) : Field(schema::IEE754, name) {}
  typedef double ValueType;
};

}
#include "schema_impl.h"
#endif
