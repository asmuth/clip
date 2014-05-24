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

namespace fnordmetric {

class IRecord {
public:

  explicit IRecord() {}

};


template<typename... T>
class Record : public IRecord {
public:

  explicit Record(const typename T::ValueType&... values) {}

};

}
#endif
