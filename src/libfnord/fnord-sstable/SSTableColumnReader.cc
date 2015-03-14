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
#include <fnord-sstable/SSTableColumnReader.h>

namespace fnord {
namespace sstable {

SSTableColumnReader::SSTableColumnReader(
    SSTableColumnSchema* schema,
    const Buffer& buf) :
    schema_(schema),
    buf_(buf),
    msg_reader_(buf_.data(), buf_.size()) {
  while (msg_reader_.remaining() > 0) {
    auto col_id = *msg_reader_.readUInt32();
    auto col_type = schema_->columnType(col_id);

    switch (col_type) {

      case SSTableColumnType::UINT32:
        col_data_.emplace_back(col_id, *msg_reader_.readUInt32(), 0);
        break;

      case SSTableColumnType::UINT64:
      case SSTableColumnType::FLOAT:
        col_data_.emplace_back(col_id, *msg_reader_.readUInt64(), 0);
        break;

      case SSTableColumnType::STRING: {
        uint32_t size = *msg_reader_.readUInt32();
        uint64_t data = (uint64_t) msg_reader_.read(size);
        col_data_.emplace_back(col_id, data, size);
        break;
      }

    }
  }
}

uint32_t SSTableColumnReader::getUInt32Column(SSTableColumnID id) {
#ifndef FNORD_NODEBUG
  if (schema_->columnType(id) != SSTableColumnType::UINT32) {
    RAISEF(kIllegalArgumentError, "invalid column type for column_id: $0", id);
  }
#endif

  for (const auto& col : col_data_) {
    if (std::get<0>(col) == id) {
      return std::get<1>(col);
    }
  }

  RAISEF(kIndexError, "no value for column: $0", id);
}

uint64_t SSTableColumnReader::getUInt64Column(SSTableColumnID id) {
#ifndef FNORD_NODEBUG
  if (schema_->columnType(id) != SSTableColumnType::UINT64) {
    RAISEF(kIllegalArgumentError, "invalid column type for column_id: $0", id);
  }
#endif

  for (const auto& col : col_data_) {
    if (std::get<0>(col) == id) {
      return std::get<1>(col);
    }
  }

  RAISEF(kIndexError, "no value for column: $0", id);
}

double SSTableColumnReader::getFloatColumn(SSTableColumnID id) {
#ifndef FNORD_NODEBUG
  if (schema_->columnType(id) != SSTableColumnType::FLOAT) {
    RAISEF(kIllegalArgumentError, "invalid column type for column_id: $0", id);
  }
#endif

  for (const auto& col : col_data_) {
    if (std::get<0>(col) == id) {
      return IEEE754::fromBytes(std::get<1>(col));
    }
  }

  RAISEF(kIndexError, "no value for column: $0", id);
}

String SSTableColumnReader::getStringColumn(SSTableColumnID id) {
  switch (schema_->columnType(id)) {
    case SSTableColumnType::UINT32:
      return StringUtil::toString(getUInt32Column(id));
    case SSTableColumnType::UINT64:
      return StringUtil::toString(getUInt64Column(id));
    case SSTableColumnType::FLOAT:
      return StringUtil::toString(getFloatColumn(id));
    case SSTableColumnType::STRING:
      break;
  }

  for (const auto& col : col_data_) {
    if (std::get<0>(col) == id) {
      return String((char*) std::get<1>(col), std::get<2>(col));
    }
  }

  RAISEF(kIndexError, "no value for column: $0", id);
}

} // namespace sstable
} // namespace fnord

