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

class RecordReader {
public:
  RecordReader(const uint8_t* data, size_t len);
  bool readInteger(int64_t* destination);
  bool readFloat(double* destination);
protected:
  const uint8_t* data_;
  size_t len_;
  size_t pos_;
};

class IRecordWriter {
public:
  explicit IRecordWriter() {}
  void appendField(double value);
  void appendField(int64_t value);
  const std::vector<uint8_t>& toBytes() const;
protected:
  std::vector<uint8_t> bytes_;
};

template<typename... T>
class RecordWriter : public IRecordWriter {
public:
  explicit RecordWriter(const typename T::ValueType&... values);
protected:
  template<typename T1> void appendFields(T1 head);
  template<typename T1, typename... T2> void appendFields(T1 head, T2... tail);
};

}

#include "record_impl.h"
#endif
