/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_STREAM_IMPL_H
#define _FNORDMETRIC_STREAM_IMPL_H

#include <stdlib.h>
#include <stdint.h>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <memory>
#include "fnv.h"

namespace fnordmetric {

template <typename... T>
StreamKey<T...>::StreamKey(
    const std::string& name,
    T... fields) :
    IStreamKey(buildKeyString(name, fields...)) {}

template <typename... T>
std::string StreamKey<T...>::buildKeyString(
    const std::string& name,
    T... fields) {
  std::stringstream ss;
  FNV<uint64_t> fnv;

  ss << std::hex << fnv.hash(name);

  buildKeyString(&ss, fields...);
  return ss.str();
}

template <typename... T>
template<typename... T1>
void StreamKey<T...>::buildKeyString(
    std::stringstream* ss,
    IField head, T1... tail) {
  FNV<uint32_t> fnv;

  *ss << '-' << 
      static_cast<char>(head.getTypeId()) <<
      std::hex << fnv.hash(head.getName());

  buildKeyString(ss, tail...);
}

template <typename... T>
void StreamKey<T...>::buildKeyString(std::stringstream* ss) {}


template <typename... T>
Stream<T...>::Stream(
    const IStreamKey& key,
    const Schema<T...>& schema,
    //const MetricDescription& description,
    std::unique_ptr<IBackend::IStreamDescriptor>&& stream_descriptor) :
    IStream(
        key,
        schema,
        //description,
        std::move(stream_descriptor)) {}

template <typename... T>
void Stream<T...>::appendRecord(const typename T::ValueType&... values) const {
  RecordWriter<T...> record(values...);
  IStream::appendRecord(record);
}

}
#endif
