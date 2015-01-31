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
#include <fnord/base/logging.h>

using fnord::Exception;

namespace fnord {

const char* logLevelToStr(LogLevel log_level) {
  switch (log_level) {
    case LogLevel::kEmergency: return "EMERGENCY";
    case LogLevel::kAlert: return "ALERT";
    case LogLevel::kCritical: return "CRITICAL";
    case LogLevel::kError: return "ERROR";
    case LogLevel::kWarning: return "WARNING";
    case LogLevel::kNotice: return "NOTICE";
    case LogLevel::kInfo: return "INFO";
    case LogLevel::kDebug: return "DEBUG";
    case LogLevel::kTrace: return "TRACE";
    default: return "CUSTOM"; // FIXPAUL
  }
}

LogLevel strToLogLevel(const String& log_level) {
  if (log_level == "EMERGENCY") return LogLevel::kEmergency;
  if (log_level == "ALERT") return LogLevel::kAlert;
  if (log_level == "CRITICAL") return LogLevel::kCritical;
  if (log_level == "ERROR") return LogLevel::kError;
  if (log_level == "WARNING") return LogLevel::kWarning;
  if (log_level == "NOTICE") return LogLevel::kNotice;
  if (log_level == "INFO") return LogLevel::kInfo;
  if (log_level == "DEBUG") return LogLevel::kDebug;
  if (log_level == "TRACE") return LogLevel::kTrace;
  RAISEF(kIllegalArgumentError, "unknown log level: $0", log_level);
}

Logger* Logger::get() {
  static Logger singleton;
  return &singleton;
}

Logger::Logger() :
    min_level_(LogLevel::kNotice),
    max_listener_index_(0) {
  for (int i = 0; i < FNORD_LOGGER_MAX_LISTENERS; ++i) {
    listeners_[i] = nullptr;
  }
}

void Logger::logException(
    LogLevel log_level,
    const String& component,
    const std::exception& exception,
    const String& message) {
  if (log_level < min_level_) {
    return;
  }

  try {
    auto rte = dynamic_cast<const fnord::Exception&>(exception);
    log(
        log_level,
        component,
        "$0: $1: $2\n    in $3\n    in $4:$5",
        message,
        rte.getTypeName(),
        rte.getMessage(),
        rte.method(),
        rte.file(),
        rte.line());
  } catch (const std::exception& bcee) {
    log(
        log_level,
        component,
        "$0: std::exception: <foreign exception> $1",
        message,
        exception.what());
  }
}

void Logger::log(
      LogLevel log_level,
      const String& component,
      const String& message) {
  if (log_level < min_level_) {
    return;
  }

  const auto max_idx = max_listener_index_.load();
  for (int i = 0; i < max_idx; ++i) {
    auto listener = listeners_[i].load();

    if (listener != nullptr) {
      listener->log(log_level, component, message);
    }
  }
}

void Logger::addTarget(LogTarget* target) {
  auto listener_id = max_listener_index_.fetch_add(1);
  listeners_[listener_id] = target;
}

void Logger::setMinimumLogLevel(LogLevel min_level) {
  min_level_ = min_level;
}


}
