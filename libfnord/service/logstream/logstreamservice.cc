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

LogStreamService::LogStreamService() {}

uint64_t LogStreamService::append(
    const std::string& stream,
    const std::string& entry) {
  fnord::iputs("append $0 -> $1", stream, entry);
  return 0;
}

} // namespace logstream_service
} // namsepace fnord
