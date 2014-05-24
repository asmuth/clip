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
#include "serialize.h"

namespace fnordmetric {

class IRecord {
public:
  explicit IRecord() {}

  const std::vector<uint8_t>& toBytes() const {
    return bytes_;
  }

  void appendField(double value) {
    printf("append double!");
    fnordmetric::serialize::toBytes(value, &bytes_);
  }

  void appendField(int64_t value) {
    printf("append field!");
    fnordmetric::serialize::toBytes(value, &bytes_);
  }

protected:
  std::vector<uint8_t> bytes_;
};


template<typename... T>
class Record : public IRecord {
public:

  explicit Record(const typename T::ValueType&... values) {
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

}
#endif
