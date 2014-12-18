/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <sstream>
#include <stdarg.h>
#include <fnord/base/exception.h>
#include <fnord/base/inspect.h>
#include <fnord/base/wallclock.h>
#include <fnord/logging/logger.h>
#include <fnord/logging/logtarget.h>

using fnord::Exception;

namespace fnord {
namespace log {

Logger* Logger::get() {
  static Logger singleton;
  return &singleton;
}

Logger::Logger() :
    min_level_(kNotice),
    max_listener_index_(0) {
  for (int i = 0; i < FNORD_LOGGER_MAX_LISTENERS; ++i) {
    listeners_[i] = nullptr;
  }
}

void Logger::log(
    LogLevel log_level,
    const std::string& message) {
  logInternal(log_level, nullptr, message);
}

void Logger::log(
    LogLevel log_level,
    const LogTags* tags,
    const std::string& message) {
  logInternal(log_level, tags, message);
}

void Logger::logException(
    LogLevel log_level,
    const std::string& message,
    const std::exception& exception) {
  logException(log_level, nullptr, message, exception);
}

void Logger::logException(
    LogLevel log_level,
    const LogTags* tags,
    const std::string& message,
    const std::exception& exception) {
  try {
    auto rte = dynamic_cast<const fnord::Exception&>(exception);
    logf(
        log_level,
        tags,
        "$0: $1\n    in $2\n    in $3:$4",
        message,
        rte.getTypeName(),
        rte.method(),
        rte.file(),
        rte.line());
  } catch (const std::exception& e) {
    logf(
        log_level,
        tags,
        "$0: std::exception: <foreign exception>",
        message);
  }
}


void Logger::logInternal(
      LogLevel log_level,
      const LogTags* tags,
      const std::string& message) {
  const auto max_idx = max_listener_index_.load();
  for (int i = 0; i < max_idx; ++i) {
    auto listener = listeners_[i].load();

    if (listener != nullptr) {
      listener->log(log_level, tags, message);
    }
  }
}

void Logger::listen(LogTarget* target) {
  auto listener_id = max_listener_index_.fetch_add(1);
  listeners_[listener_id] = target;
}

void Logger::setMinimumLogLevel(LogLevel min_level) {
  min_level_ = min_level;
}

/*
void Logger::log(
    const std::string& severity,
    const std::string& message) {
  LogEntry entry;
  entry.append("__severity__", severity);
  entry.append("__message__", message);
  log(entry);
}

void Logger::printf(const std::string& key, std::string value, ...) {
  char buf[4096]; // FIXPAUL!
  buf[0] = 0;

  va_list args;
  va_start(args, value);
  vsnprintf(buf, sizeof(buf), value.c_str(), args);
  va_end(args);

  log(key, buf);
}
  try {
    auto rte = dynamic_cast<const fnord::Exception&>(error);
    fprintf(stderr, "ERROR: Uncaught exception\n");
    rte.debugPrint();
  } catch (const std::exception& cast_error) {
    fprintf(stderr, "ERROR: Uncaught foreign exception: %s\n", error.what());
  }
LogEntry::LogEntry() : time_(WallClock::now()) {}

void LogEntry::append(const std::string& key, const std::string& value) {
  lines_.emplace_back(key, value);
}

void LogEntry::printf(const std::string& key, std::string value, ...) {
  char buf[4096]; // FIXPAUL!
  buf[0] = 0;

  va_list args;
  va_start(args, value);
  vsnprintf(buf, sizeof(buf), value.c_str(), args);
  va_end(args);

  append(key, buf);
}

const std::vector<std::pair<std::string, std::string>>& LogEntry::lines()
    const {
  return lines_;
}

const DateTime& LogEntry::time() const {
  return time_;
}
*/

}
}
