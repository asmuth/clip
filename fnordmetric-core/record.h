/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_RECORD_H
#define _FNORDMETRIC_RECORD_H

#include <stdlib.h>
#include <stdint.h>
#include "schema.h"

namespace fnordmetric {

class IRecordWriter {
public:
  explicit IRecordWriter() {}

  const std::vector<uint8_t>& toBytes() const {
    return bytes_;
  }

  void appendField(double value) {
    bytes_.emplace_back(schema::IEE754);
    bytes_.emplace_back(0);
    bytes_.emplace_back(0);
    bytes_.emplace_back(0);
    bytes_.emplace_back(0);
    bytes_.emplace_back(0);
    bytes_.emplace_back(0);
    bytes_.emplace_back(0);
    bytes_.emplace_back(0);
  }

  void appendField(int64_t value) {
    int64_t local_value = value;
    uint8_t bytes[8];
    memcpy(bytes, &local_value, 8);
    bytes_.emplace_back(schema::INT64);
    bytes_.emplace_back(bytes[0]);
    bytes_.emplace_back(bytes[1]);
    bytes_.emplace_back(bytes[2]);
    bytes_.emplace_back(bytes[3]);
    bytes_.emplace_back(bytes[4]);
    bytes_.emplace_back(bytes[5]);
    bytes_.emplace_back(bytes[6]);
    bytes_.emplace_back(bytes[7]);
  }

protected:
  std::vector<uint8_t> bytes_;
};


template<typename... T>
class RecordWriter : public IRecordWriter {
public:

  explicit RecordWriter(const typename T::ValueType&... values) {
    appendFields(values...);
  }

protected:

  template <typename T1>
  void appendFields(T1 head) {
    appendField(head);
  }

  template <typename T1, typename... T2>
  void appendFields(T1 head, T2... tail) {
    appendField(head);
    appendFields(tail...);
  }

};

class RecordReader {
public:

  RecordReader(const uint8_t* data, size_t len) :
      data_(data),
      len_(len),
      pos_(0) {}

  bool readInteger(int64_t* destination) {
    int64_t value;
    if (pos_ + 9 > len_ || data_[pos_] != schema::INT64) {
      return false;
    }

    memcpy(destination, data_ + pos_ + 1, 8);
    pos_ += 9;
    return true;
  }

  bool readFloat(double* destination) {
    if (pos_ + 9 > len_ || data_[pos_] != schema::IEE754) {
      return false;
    }

    *destination = 23.5;
    return true;
  }

protected:
  const uint8_t* data_;
  size_t len_;
  size_t pos_;
};


}
#endif
