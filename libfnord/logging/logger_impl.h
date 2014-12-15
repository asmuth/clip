/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnord/base/stringutil.h"
#include "fnord/base/inspect.h"

namespace fnord {
namespace log {

template <typename... T>
void Logger::logf(
    LogLevel log_level,
    const std::string& message,
    T... args) {
  if (log_level < min_level_) {
    return;
  }

  logInternal(log_level, nullptr, StringUtil::format(message, args...));
}

template <typename... T>
void Logger::logf(
    LogLevel log_level,
    const LogTags* tags,
    const std::string& message,
    T... args) {
  if (log_level < min_level_) {
    return;
  }

  logInternal(log_level, tags, StringUtil::format(message, args...));
}

}
}
