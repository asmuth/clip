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
class Field;

/**
 * Define the Field types
 */
namespace schema {
enum kFieldType {
  INT64  = 0,
  IEE754 = 1,
  STRING = 2
};

static const char kFieldTypesHuman[] = {
  'I', /* Integer */
  'F', /* Float */
  'S'  /* String */
};

static const size_t kFieldTypesSize[] = {
  8, /* Integer */
  8, /* Float */
  8  /* String */
};

}

/**
 * A schema is an ordered flat list of fields. Each field consists of a type and
 * a name. The type can be one of Integer, Float, String and the name must be a
 * string. Nested fields are not supported. Schemas are immutable.
 */
class Schema {
public:

  /**
   * Initialize a new schema with the specified fields
   */
  explicit Schema(const std::vector<Field>& fields);

  /**
   * The field list of this schema
   */
  const std::vector<Field> fields_;

};

class Field {
public:
  const std::string& getName() const;
  schema::kFieldType getTypeId() const;
protected:
  Field(schema::kFieldType type, const std::string& name);
  schema::kFieldType type_; // FIXPAUL REMOVE ME
  std::string name_;
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
