/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_LOGSTREAM_SERVICE_LOGSTREAM_H
#define _FNORD_LOGSTREAM_SERVICE_LOGSTREAM_H
#include <mutex>
#include <stdlib.h>
#include <set>
#include <string>
#include <unordered_map>
#include "fnord/io/filerepository.h"
#include "fnord/reflect/reflect.h"
#include "fnord/service/logstream/logstreamentry.h"
#include "fnord/sstable/sstablewriter.h"

namespace fnord {
namespace logstream_service {

class LogStream {
public:
  static const size_t kMaxTableSize = 1024;

  LogStream(const std::string& name, io::FileRepository* file_repo);

  uint64_t append(const std::string& entry);

  std::vector<LogStreamEntry> fetch(uint64_t offset, int batch_size);

  struct TableHeader {
    uint64_t offset;
    std::string stream_name;

    template <typename T>
    static void reflect(T* meta) {
      meta->prop(&TableHeader::offset, 1, "offset", false);
      meta->prop(&TableHeader::stream_name, 2, "stream_name", false);
    }
  };

  void reopenTable(const std::string& file_path);

protected:

  struct TableRef {
    uint64_t offset;
    std::string file_path;
    std::unique_ptr<sstable::SSTableWriter> writer;
  };

  std::shared_ptr<TableRef> createTable();
  size_t getTableBodySize(const std::string& file_path);

  std::string name_;
  io::FileRepository* file_repo_;
  std::vector<std::shared_ptr<TableRef>> tables_;
  std::mutex tables_mutex_;
};

} // namespace logstream_service
} // namespace fnord
#endif
