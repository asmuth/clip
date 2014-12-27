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
#include "fnord/sstable/sstablereader.h"
#include "fnord/sstable/sstablerepair.h"
#include "fnord/service/logstream/logstreamservice.h"

namespace fnord {
namespace logstream_service {

LogStreamService::LogStreamService(
    fnord::io::FileRepository file_repo) :
    file_repo_(file_repo) {
  file_repo.listFiles([this] (const std::string& filename) -> bool {
    reopenTable(filename);
    return true;
  });
}

uint64_t LogStreamService::append(std::string stream_key, std::string entry) {
  auto stream = openStream(stream_key, true);
  return stream->append(entry);
}

std::vector<LogStreamEntry> LogStreamService::fetch(
      std::string stream_key,
      uint64_t offset,
      int batch_size) {
  auto stream = openStream(stream_key, false);
  return stream->fetch(offset, batch_size);
}

LogStream* LogStreamService::openStream(const std::string& name, bool create) {
  std::unique_lock<std::mutex> l(streams_mutex_);

  LogStream* stream = nullptr;

  auto stream_iter = streams_.find(name);
  if (stream_iter == streams_.end()) {
    if (!create) {
      RAISEF(kIndexError, "no such stream: $0", name);
    }

    stream = new LogStream(name, &file_repo_);
    streams_.emplace(name, stream);
  } else {
    stream = stream_iter->second.get();
  }

  return stream;
}

void LogStreamService::reopenTable(const std::string& file_path) {
  fnord::sstable::SSTableRepair repair(file_path);
  if (!repair.checkAndRepair(true)) {
    RAISEF(kRuntimeError, "corrupt sstable: $0", file_path);
  }

  auto file = io::File::openFile(file_path, io::File::O_READ);
  sstable::SSTableReader reader(std::move(file));

  auto table_header = fnord::json::fromJSON<LogStream::TableHeader>(
      reader.readHeader());

  if (reader.bodySize() == 0) {
    auto writer = sstable::SSTableWriter::reopen(
        file_path,
        sstable::IndexProvider{});
    writer->finalize();
  }

  auto stream = openStream(table_header.stream_name, true);
  stream->reopenTable(file_path);
}

} // namespace logstream_service
} // namsepace fnord
