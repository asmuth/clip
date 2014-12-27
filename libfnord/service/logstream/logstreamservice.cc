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
#include "fnord/service/logstream/logstreamservice.h"

namespace fnord {
namespace logstream_service {

LogStreamService::LogStreamService(
    fnord::io::FileRepository file_repo) :
    file_repo_(file_repo) {}

uint64_t LogStreamService::append(std::string stream_key, std::string entry) {
  auto stream = openStream(stream_key);
  return stream->append(entry);
}

LogStream* LogStreamService::openStream(const std::string& name) {
  std::unique_lock<std::mutex> l(streams_mutex_);

  LogStream* stream = nullptr;

  auto stream_iter = streams_.find(name);
  if (stream_iter == streams_.end()) {
    stream = new LogStream(name, &file_repo_);
    streams_.emplace(name, stream);
  } else {
    stream = stream_iter->second.get();
  }

  return stream;
}

} // namespace logstream_service
} // namsepace fnord
