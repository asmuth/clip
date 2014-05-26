/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include "stream.h"

namespace fnordmetric {

const std::string& IStreamKey::getKeyString() const {
  return key_str_;
}

IStream::IStream(
    const IStreamKey& key,
    const ISchema& schema,
    //const MetricDescription& description,
    std::unique_ptr<IBackend::IStreamDescriptor>&& stream_descriptor) :
    key_(key),
    schema_(schema),
    //description_(description),
    stream_descriptor_(std::move(stream_descriptor)) {}

const ISchema& IStream::getSchema() const {
  return schema_;
}

const IStreamKey& IStream::getKey() const {
  return key_;
}

std::unique_ptr<IBackend::IStreamCursor> IStream::getCursor() const {
  return stream_descriptor_->getCursor();
}

void IStream::appendRecord(const IRecordWriter& record) const {
  stream_descriptor_->appendRow(record.toBytes());
}

}
