/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnord-sstable/SSTableColumnSchema.h>
#include <fnord-sstable/sstablereader.h>
#include <fnord-sstable/sstablewriter.h>
#include <fnord-base/util/binarymessagereader.h>
#include <fnord-base/util/binarymessagewriter.h>

namespace fnord {
namespace sstable {

SSTableColumnSchema::SSTableColumnSchema() {}

void SSTableColumnSchema::addColumn(
    const String& name,
    uint32_t id,
    SSTableColumnType type) {
  SSTableColumnInfo info;
  info.name = name;
  info.type = type;
  col_info_[id] = info;
  col_ids_[name] = id;
}

SSTableColumnType SSTableColumnSchema::columnType(SSTableColumnID id) const {
  auto iter = col_info_.find(id);
  if (iter == col_info_.end()) {
    RAISEF(kIndexError, "invalid column index: $0", id);
  }

  return iter->second.type;
}

String SSTableColumnSchema::columnName(SSTableColumnID id) const {
  auto iter = col_info_.find(id);
  if (iter == col_info_.end()) {
    RAISEF(kIndexError, "invalid column index: $0", id);
  }

  return iter->second.name;
}

SSTableColumnID SSTableColumnSchema::columnID(const String& column_name) const {
  auto iter = col_ids_.find(column_name);
  if (iter == col_ids_.end()) {
    RAISEF(kIndexError, "invalid column: $0", column_name);
  }

  return iter->second;
}

Set<SSTableColumnID> SSTableColumnSchema::columnIDs() const {
  Set<SSTableColumnID> ids;

  for (const auto& c : col_info_) {
    ids.emplace(c.first);
  }

  return ids;
}

void SSTableColumnSchema::writeIndex(Buffer* buf) {
  util::BinaryMessageWriter writer;

  for (const auto& c : col_info_) {
    writer.appendUInt32((uint8_t) c.second.type);
    writer.appendUInt32(c.first);
    writer.appendUInt32(c.second.name.length());
    writer.append(c.second.name.data(), c.second.name.length());
  }

  buf->append(writer.data(), writer.size());
}

void SSTableColumnSchema::writeIndex(SSTableWriter* sstable_writer) {
  Buffer buf;
  writeIndex(&buf);

  sstable_writer->writeIndex(SSTableColumnSchema::kSSTableIndexID, buf);
}

void SSTableColumnSchema::loadIndex(const Buffer& buf) {
  util::BinaryMessageReader reader(buf.data(), buf.size());

  while (reader.remaining() > 0) {
    uint32_t col_type = *reader.readUInt32();
    uint32_t col_id = *reader.readUInt32();
    uint32_t col_name_len = *reader.readUInt32();
    String col_name((char*) reader.read(col_name_len), col_name_len);

    addColumn(col_name, col_id, (fnord::sstable::SSTableColumnType) col_type);
  }
}

void SSTableColumnSchema::loadIndex(
    SSTableReader* sstable_reader) {
  auto index = sstable_reader->readFooter(kSSTableIndexID);
  loadIndex(index);
}

} // namespace sstable
} // namespace fnord
