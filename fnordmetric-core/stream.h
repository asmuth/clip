/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_STREAM_H
#define _FNORDMETRIC_STREAM_H

#include <stdlib.h>
#include <stdint.h>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <memory>
#include "schema.h"
#include "record.h"
#include "database/streamref.h"

namespace fnordmetric {

class StreamKey {
public:
  //explicit IStreamKey(const std:string& name, const ISchema& schema);
  const std::string& getKeyString() const;
protected:
  explicit StreamKey(const std::string& key_str) : key_str_(key_str) {}
  const std::string key_str_;
};

template <typename... T>
class TypedStreamKey : public StreamKey {
public:
  explicit TypedStreamKey(const std::string& name, T... fields);
protected:
  std::string buildKeyString(const std::string& name, T... fields);
  template<typename... T1>
  void buildKeyString(std::stringstream* ss, Field head, T1... tail);
  void buildKeyString(std::stringstream* ss);
};

class Stream {
public:

  explicit Stream(
      const StreamKey& key,
      const Schema& schema,
      //const MetricDescription& description,
      std::shared_ptr<database::StreamRef> stream_ref);

  Stream(const Stream& copy) = delete;
  Stream& operator=(const Stream& copy) = delete;

  const Schema& getSchema() const;
  const StreamKey& getKey() const;

  void appendRecord(const RecordWriter& record) const;

protected:
  const std::shared_ptr<database::StreamRef> stream_ref_;
  const StreamKey key_;
  //const MetricDescription description_;
  const Schema schema_;
};

template <typename... T>
class TypedStream : public Stream {
public:

  explicit TypedStream(
      const StreamKey& key,
      const TypedSchema<T...>& schema,
      //const MetricDescription& description,
      std::shared_ptr<database::StreamRef> stream_ref);

  TypedStream(const TypedStream& copy) = delete;
  TypedStream& operator=(const TypedStream& copy) = delete;

  void appendRecord(const typename T::ValueType&... values) const;
};

}
#include "stream_impl.h"
#endif
