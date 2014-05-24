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

class IField {
public:
  IField(schema::kFieldType type, const std::string& name);
  const std::string& getName() const;
  schema::kFieldType getTypeId() const;
protected:
  schema::kFieldType type_; // FIXPAUL REMOVE ME
  std::string name_;
};

class ISchema {
public:
  explicit ISchema(const std::vector<IField>& fields);
  const std::vector<IField>& getFields() const;
protected:
  const std::vector<IField> fields_;
};

template <typename... T>
class Schema : public ISchema {
public:
  explicit Schema(const T... fields);
protected:
  std::vector<IField> unpackFields(T... fields);
};

class IntegerField : public IField {
public:
  IntegerField(const std::string& name) : IField(schema::INT64, name) {}
  typedef int64_t ValueType;
};

class FloatField : public IField {
public:
  FloatField(const std::string& name) : IField(schema::IEE754, name) {}
  typedef double ValueType;
};

}
#include "schema_impl.h"
#endif
