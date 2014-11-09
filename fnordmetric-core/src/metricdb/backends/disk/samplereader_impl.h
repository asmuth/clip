/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_METRICDB_SAMPLEREADER_IMPL_H
#define _FNORDMETRIC_METRICDB_SAMPLEREADER_IMPL_H

namespace fnordmetric {
namespace metricdb {
namespace disk_backend {

template <typename T>
SampleReader<T>::SampleReader(
    void* data,
    size_t size,
    TokenIndex* token_index) :
    AbstractSampleReader(data, size, token_index) {
  value_ = readValue();
}

template <typename T>
const T& SampleReader<T>::value() {
  return value_;
}


}
}
}

#endif
