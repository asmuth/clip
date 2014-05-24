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
#include "serialize.h"

namespace fnordmetric {
class Agent;

namespace schema {
enum kFieldType {
  INT64  = 'I',
  IEE754 = 'F',
  STRING = 'S'
};
}

class IField {
public:
  IField(schema::kFieldType type, const std::string& name) :
      type_(type),
      name_(name) {}

  const std::string& getName() const {
    return name_;
  }

  schema::kFieldType getTypeId() const {
    return type_;
  }

protected:
  schema::kFieldType type_; // FIXPAUL REMOVE ME
  std::string name_;
};


class ISchema {
public:
  explicit ISchema(const std::vector<IField>& fields) : fields_(fields) {}

  const std::vector<IField>& getFields() const {
    return fields_;
  }

protected:
  const std::vector<IField> fields_;
};


template <typename... T>
class Schema : public ISchema {
public:

  explicit Schema(const T... fields) :
      ISchema(unpackFields(fields...)) {}

protected:

  // FIXPAUL there must be some better way to do this...
  std::vector<IField> unpackFields(T... fields) {
    std::vector<IField> unpacked;
    const IField packed[] = {fields...};
    int num_fields = sizeof(packed) / sizeof(IField);

    for (int i = 0; i < num_fields; ++i) {
      unpacked.push_back(packed[i]);
    }

    return unpacked;
  };

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
#endif
