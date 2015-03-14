/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnord-base/ieee754.h>
#include <fnord-sstable/SSTableColumnWriter.h>

namespace fnord {
namespace sstable {

SSTableColumnWriter::SSTableColumnWriter(
    SSTableColumnSchema* schema) :
    schema_(schema) {}

void SSTableColumnWriter::addUInt32Column(SSTableColumnID id, uint32_t value) {
#ifndef FNORD_NODEBUG
  if (schema_->columnType(id) != SSTableColumnType::UINT32) {
    RAISEF(kIllegalArgumentError, "invalid column type for column_id: $0", id);
  }
#endif

  msg_writer_.appendUInt32(id);
  msg_writer_.appendUInt32(value);
}

void SSTableColumnWriter::addUInt64Column(SSTableColumnID id, uint64_t value) {
#ifndef FNORD_NODEBUG
  if (schema_->columnType(id) != SSTableColumnType::UINT64) {
    RAISEF(kIllegalArgumentError, "invalid column type for column_id: $0", id);
  }
#endif

  msg_writer_.appendUInt32(id);
  msg_writer_.appendUInt64(value);
}

void SSTableColumnWriter::addFloatColumn(SSTableColumnID id, double value) {
#ifndef FNORD_NODEBUG
  if (schema_->columnType(id) != SSTableColumnType::FLOAT) {
    RAISEF(kIllegalArgumentError, "invalid column type for column_id: $0", id);
  }
#endif

  msg_writer_.appendUInt32(id);
  msg_writer_.appendUInt64(IEEE754::toBytes(value));
}

void SSTableColumnWriter::addStringColumn(
    SSTableColumnID id,
    const String& value) {
#ifndef FNORD_NODEBUG
  if (schema_->columnType(id) != SSTableColumnType::STRING) {
    RAISEF(kIllegalArgumentError, "invalid column type for column_id: $0", id);
  }
#endif

  uint32_t len = value.length();
  msg_writer_.appendUInt32(id);
  msg_writer_.appendUInt32(len);
  msg_writer_.append(value.data(), len);
}

void* SSTableColumnWriter::data() const {
  return msg_writer_.data();
}

size_t SSTableColumnWriter::size() const {
  return msg_writer_.size();
}

} // namespace sstable
} // namespace fnord
