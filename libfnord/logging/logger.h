/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_UTIL_LOGGER_H
#define _FNORDMETRIC_UTIL_LOGGER_H
#include <fnord/base/datetime.h>
#include <string>
#include <vector>

namespace fnord {
namespace log {

#ifndef FNORD_LOGGER_MAX_LISTENERS
#define FNORD_LOGGER_MAX_LISTENERS 64
#endif

class Logger {
public:
  Logger() {}
  virtual ~Logger() {}

  void log(
      kLogLevel log_level,
      const std::string& message);

  void log(
      LogLevel log_level,
      const LogTags* tags,
      const std::string& message);

  template <typename T...>
  void logf(
      LogLevel log_level,
      const std::string& message,
      T... args);

  template <typename T...>
  void logf(
      LogLevel log_level,
      const LogTags* tags,
      const std::string& message,
      T... args);

  void logException(
      LogLevel log_level,
      const LogTags* tags,
      const std::exception& exception);

  void logException(
      LogLevel log_level,
      const std::exception& exception);

  void logException(
      LogLevel log_level,
      const std::string& message,
      const std::exception& exception);

  void logException(
      LogLevel log_level,
      const LogTags* tags,
      const std::string& message,
      const std::exception& exception);

  void printf(
      LogLevel log_level,
      std::string message,
      ...);

  void listen(LogLevel min_level, LogTarget* target);

protected:
  void setMinimumLogLevel(LogLevel min_level);
  std::atomic<LogLevel> min_level_;
  std::atomic<size_t> max_listener_index_;
  std::atomic<LogTarget*> listeners_[FNORD_LOGGER_MAX_LISTENERS];
};

}
}
#endif
