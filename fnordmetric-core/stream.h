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
#include "storagebackend.h"

namespace fnordmetric {

class IStreamKey {
public:
  //explicit IStreamKey(const std:string& name, const ISchema& schema);
  const std::string& getKeyString() const;
protected:
  explicit IStreamKey(const std::string& key_str) : key_str_(key_str) {}
  const std::string key_str_;
};

template <typename... T>
class StreamKey : public IStreamKey {
public:
  explicit StreamKey(const std::string& name, T... fields);
protected:
  std::string buildKeyString(const std::string& name, T... fields);
  template<typename... T1>
  void buildKeyString(std::stringstream* ss, IField head, T1... tail);
  void buildKeyString(std::stringstream* ss);
};

class IStream {
public:

  explicit IStream(
      const IStreamKey& key,
      const ISchema& schema,
      //const MetricDescription& description,
      std::unique_ptr<IBackend::IStreamDescriptor>&& stream_descriptor);

  IStream(const IStream& copy) = delete;
  IStream& operator=(const IStream& copy) = delete;

  const ISchema& getSchema() const;
  const IStreamKey& getKey() const;
  std::unique_ptr<IBackend::IStreamCursor> getCursor() const;

  void appendRecord(const IRecordWriter& record) const;

protected:
  const std::unique_ptr<IBackend::IStreamDescriptor> stream_descriptor_;
  const IStreamKey key_;
  //const MetricDescription description_;
  const ISchema schema_;
};

template <typename... T>
class Stream : public IStream {
public:

  explicit Stream(
      const IStreamKey& key,
      const Schema<T...>& schema,
      //const MetricDescription& description,
      std::unique_ptr<IBackend::IStreamDescriptor>&& stream_descriptor);

  Stream(const Stream& copy) = delete;
  Stream& operator=(const Stream& copy) = delete;

  void appendRecord(const typename T::ValueType&... values) const;
};

}
#include "stream_impl.h"
#endif
