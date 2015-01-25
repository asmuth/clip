/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <algorithm>
#include "fnord/base/inspect.h"
#include "fnord/base/logging.h"
#include "fnord/json/json.h"
#include "fnord/sstable/sstablereader.h"
#include "fnord-feeds/LocalFeed.h"
#include "fnord-feeds/FeedService.h"
#include "fnord/stats/stats.h"

namespace fnord {
namespace feeds {

LogStream::LogStream(
    const std::string& name,
    FeedService* base) :
    name_(name),
    base_(base) {
  exportStat(
      StringUtil::format("$0/$1/$2", base_->stats_path_, name_, "head_offset"),
      &stat_head_offset_,
      stats::ExportMode::EXPORT_VALUE);
}

uint64_t LogStream::append(const std::string& entry) {
  std::unique_lock<std::mutex> l(tables_mutex_);

  if (tables_.size() == 0 || tables_.back()->writer.get() == nullptr) {
    tables_.emplace_back(createTable());
  }

  const auto& tbl = tables_.back();
  auto row_offset = tbl->writer->appendRow("", entry);

  if (row_offset > kMaxTableSize) {
    tbl->writer->finalize();
    tbl->writer.reset(nullptr);
  }

  head_offset_ = tbl->offset + row_offset;
  stat_head_offset_.set(head_offset_);
  return head_offset_;
}

std::vector<FeedEntry> LogStream::fetch(uint64_t offset, int batch_size) {
  std::vector<FeedEntry> entries;
#ifndef FNORD_NOTRACE
  auto request_id = rnd_.hex64();
#endif

#ifndef FNORD_NOTRACE
  fnord::logTrace(
      "fnord.feeds.localfeed",
      "request id=$0 feed=$1 offset=$2 batch_size=$3",
      request_id,
      name_,
      offset,
      batch_size);
#endif

  std::shared_ptr<TableRef> table(nullptr);
  {
    std::unique_lock<std::mutex> l(tables_mutex_);

#ifndef FNORD_NOTRACE
    if (tables_.empty()) {
      fnord::logTrace(
          "fnord.feeds.localfeed",
          "request id=$0: feed has no tables",
          request_id);
#endif

      return entries;
    }

    if (offset == 0) {
      table = tables_[0];
    } else {
      for (int i = tables_.size() - 1; i >= 0; --i) {
        if (tables_[i]->offset <= offset) {
          table = tables_[i];
          break;
        }
      }
    }
  }

  if (table.get() == nullptr) {
    RAISEF(kIndexError, "invalid offset: $0 (stream: $1)", offset, name_);
  }

#ifndef FNORD_NOTRACE
  fnord::logTrace(
      "fnord.feeds.localfeed",
      "request id=$0: choosing table: $1",
      request_id,
      table->file_path);
#endif

  std::unique_ptr<sstable::Cursor> cursor;
  std::unique_ptr<sstable::SSTableReader> reader;

  if (table->writer.get() == nullptr) {
    reader.reset(new sstable::SSTableReader(
        File::openFile(table->file_path, File::O_READ)));

    cursor = reader->getCursor();
  } else {
    cursor = table->writer->getCursor();
  }

  if (offset == std::numeric_limits<uint64_t>::max()) {
    offset = head_offset_;
  }

  if (offset > 0) {
#ifndef FNORD_NOTRACE
    fnord::logTrace(
        "fnord.feeds.localfeed",
        "request id=$0: seeking to table_offset=$1 logical_offset=$2",
        request_id,
        offset - table->offset,
        offset);
#endif

    if (!cursor->trySeekTo(offset - table->offset)) {
#ifndef FNORD_NOTRACE
      fnord::logTrace(
          "fnord.feeds.localfeed",
          "request id=$0: seek to target offset failed",
          request_id);
#endif

      return entries;
    }
  }

  for (int i = 0; i < batch_size; i++) {
#ifndef FNORD_NOTRACE
    fnord::logTrace(
        "fnord.feeds.localfeed",
        "request id=$0: reading entry at table_offset=$1 "
            "logical_offset=$2",
        request_id,
        cursor->position(),
        table->offset + cursor->position());
#endif

    if (!cursor->valid()) {
      if (i == 0) {
        RAISEF(kIndexError, "invalid offset: $0 (stream: $1)", offset, name_);
      }

      break;
    }

    FeedEntry entry;
    entry.offset = table->offset + cursor->position();
    entry.next_offset = table->offset + cursor->nextPosition();
    entry.data = cursor->getDataString();
    entry.time = 0;
    entries.emplace_back(std::move(entry));

    if (!cursor->next()) {
      break;
    }
  }

#ifndef FNORD_NOTRACE
  fnord::logTrace(
      "fnord.feeds.localfeed",
      "request id=$0: returning $1 entries",
      request_id,
      entries.size());
#endif

  return entries;
}

std::shared_ptr<LogStream::TableRef> LogStream::createTable() {
  std::shared_ptr<TableRef> table(new TableRef());

  if (tables_.empty()) {
    table->offset = 0;
  } else {
    const auto& t = tables_.back();
    table->offset = t->offset + getTableBodySize(t->file_path);
  }

  auto file = base_->file_repo_.createFile();
  table->file_path = file.absolute_path;

  TableHeader tbl_header;
  tbl_header.offset = table->offset;
  tbl_header.stream_name = name_;
  auto tbl_header_json = fnord::json::toJSONString(tbl_header);

  File::openFile(
      table->file_path,
      File::O_READ | File::O_WRITE | File::O_CREATE);

  table->writer = sstable::SSTableWriter::create(
      table->file_path,
      sstable::IndexProvider{},
      tbl_header_json.c_str(),
      tbl_header_json.length());

  return table;
}

void LogStream::reopenTable(const std::string& file_path) {
  auto file = File::openFile(file_path, File::O_READ);
  sstable::SSTableReader reader(std::move(file));

  auto table_header = fnord::json::fromJSON<LogStream::TableHeader>(
      reader.readHeader());

  auto tbl = new TableRef();
  tbl->offset = table_header.offset;
  tbl->file_path = file_path;
  tbl->writer.reset(nullptr);

  std::unique_lock<std::mutex> l(tables_mutex_);
  tables_.emplace_back(tbl);

  std::sort(tables_.begin(), tables_.end(), [] (
      std::shared_ptr<TableRef> t1,
      std::shared_ptr<TableRef> t2) {
    return t1->offset < t2->offset;
  });

  auto this_head_offset = tbl->offset + reader.bodySize();
  if (this_head_offset > head_offset_) {
    head_offset_ = this_head_offset;
    stat_head_offset_.set(head_offset_);
  }
}

size_t LogStream::getTableBodySize(const std::string& file_path) {
  auto file = File::openFile(file_path, File::O_READ);
  sstable::SSTableReader reader(std::move(file));
  return reader.bodySize();
}

} // namespace logstream_service
} // namespace fnord

