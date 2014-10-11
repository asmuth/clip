/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include "stream.h"

namespace fnordmetric {

const std::string& StreamKey::getKeyString() const {
  return key_str_;
}

Stream::Stream(
    const StreamKey& key,
    const Schema& schema,
    //const MetricDescription& description,
    std::shared_ptr<database::StreamRef> stream_ref) :
    key_(key),
    schema_(schema),
    //description_(description),
    stream_ref_(std::move(stream_ref)) {}

const Schema& Stream::getSchema() const {
  return schema_;
}

const StreamKey& Stream::getKey() const {
  return key_;
}

void Stream::appendRecord(const RecordWriter& record) const {
  stream_ref_->appendRow(record);
}

}
