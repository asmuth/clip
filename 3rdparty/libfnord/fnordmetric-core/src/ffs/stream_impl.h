/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
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
TypedStreamKey<T...>::TypedStreamKey(
    const std::string& name,
    T... fields) :
    StreamKey(buildKeyString(name, fields...)) {}

template <typename... T>
std::string TypedStreamKey<T...>::buildKeyString(
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
void TypedStreamKey<T...>::buildKeyString(
    std::stringstream* ss,
    Field head, T1... tail) {
  FNV<uint32_t> fnv;

  *ss << '-' << 
      static_cast<char>(head.getTypeId()) <<
      std::hex << fnv.hash(head.getName());

  buildKeyString(ss, tail...);
}

template <typename... T>
void TypedStreamKey<T...>::buildKeyString(std::stringstream* ss) {}


template <typename... T>
TypedStream<T...>::TypedStream(
    const StreamKey& key,
    const TypedSchema<T...>& schema,
    //const MetricDescription& description,
    std::shared_ptr<database::StreamRef> stream_ref) :
    Stream(
        key,
        schema,
        //description,
        std::move(stream_ref)) {}

template <typename... T>
void TypedStream<T...>::appendRecord
    (const typename T::ValueType&... values) const {
  TypedRecordWriter<T...> record(values...);
  Stream::appendRecord(record);
}

}
#endif
