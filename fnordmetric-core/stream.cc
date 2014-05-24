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
#include "storagebackend.h"

namespace fnordmetric {

const std::string& IStreamKey::getKeyString() const {
  return key_str_;
}

IStream::IStream(
    const IStreamKey& key,
    const ISchema& schema,
    //const MetricDescription& description,
    std::unique_ptr<IStorageCursor>&& cursor) :
    key_(key),
    schema_(schema),
    //description_(description),
    cursor_(std::move(cursor)) {}

const ISchema& IStream::getSchema() const {
  return schema_;
}

const IStreamKey& IStream::getKey() const {
  return key_;
}

std::unique_ptr<IStorageCursor> IStream::getCursor() const {
  return cursor_->clone();
}

void IStream::appendRecord(const IRecordWriter& record) const {
  assert(cursor_.get() != nullptr);
  cursor_->appendRow(record.toBytes());
}
  
}
