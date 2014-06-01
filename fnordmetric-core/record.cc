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

void RecordWriter::appendField(double value) {
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

void RecordWriter::appendField(int64_t value) {
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

void RecordWriter::toBytes(const void** data, size_t* size) const {
  *data = bytes_.data();
  *size = bytes_.size();
}

RecordReader::RecordReader(const uint8_t* data, size_t len) :
    data_(data),
    len_(len),
    pos_(0) {}

bool RecordReader::readInteger(int64_t* destination) {
  int64_t value;
  if (pos_ + 9 > len_ || data_[pos_] != schema::INT64) {
    return false;
  }

  memcpy(destination, data_ + pos_ + 1, 8);
  pos_ += 9;
  return true;
}

bool RecordReader::readFloat(double* destination) {
  if (pos_ + 9 > len_ || data_[pos_] != schema::IEE754) {
    return false;
  }

  *destination = 23.5;
  return true;
}

}
