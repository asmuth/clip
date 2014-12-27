/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnord/base/inspect.h"
#include "fnord/json/json.h"
#include "fnord/service/logstream/logstream.h"

namespace fnord {
namespace logstream_service {

LogStream::LogStream(
    const std::string& name,
    io::FileRepository* file_repo) :
    name_(name),
    file_repo_(file_repo) {}

uint64_t LogStream::append(const std::string& entry) {
  auto head_table = headTable();
  std::unique_lock<std::mutex> l(head_table->writer_mutex_);
  return head_table->writer->appendRow("", entry);
}

std::shared_ptr<LogStream::TableRef> LogStream::headTable() {
  std::unique_lock<std::mutex> l(tables_mutex_);

  if (tables_.size() == 0 || tables_.back()->writer.get() == nullptr) {
    tables_.emplace_back(createTable());
  }

  return tables_.back();
}

std::shared_ptr<LogStream::TableRef> LogStream::createTable() {
  std::shared_ptr<TableRef> table(new TableRef());
  table->offset = 0;

  auto file = file_repo_->createFile();
  table->file_path = file.absolute_path;

  TableHeader tbl_header;
  tbl_header.offset = table->offset;
  tbl_header.stream_name = name_;
  auto tbl_header_json = fnord::json::toJSONString(tbl_header);

  io::File::openFile(
      table->file_path,
      io::File::O_READ | io::File::O_WRITE | io::File::O_CREATE);

  table->writer = sstable::SSTableWriter::create(
      table->file_path,
      sstable::IndexProvider{},
      tbl_header_json.c_str(),
      tbl_header_json.length());

  return table;
}

} // namespace logstream_service
} // namespace fnord

