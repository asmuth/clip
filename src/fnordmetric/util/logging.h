/**
 * Copyright (c) 2016 DeepCortex GmbH <legal@eventql.io>
 * Authors:
 *   - Paul Asmuth <paul@eventql.io>
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License ("the license") as
 * published by the Free Software Foundation, either version 3 of the License,
 * or any later version.
 *
 * In accordance with Section 7(e) of the license, the licensing of the Program
 * under the license does not imply a trademark license. Therefore any rights,
 * title and interest in our trademarks remain entirely with us.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the license for more details.
 *
 * You can be released from the requirements of the license by purchasing a
 * commercial license. Buying such a license is mandatory as soon as you develop
 * commercial activities involving this program without disclosing the source
 * code of your own applications
 */
#pragma once
#include <string>
#include <sstream>
#include <stdarg.h>
#include <atomic>
#include <memory>
#include <mutex>
#include "stringutil.h"

enum class LogLevel {
  kFatal = 10000,
  kEmergency = 9000,
  kAlert = 8000,
  kCritical = 7000,
  kError = 6000,
  kWarning = 5000,
  kNotice = 4000,
  kInfo = 3000,
  kDebug = 2000,
  kTrace = 1000
};

class LogTarget {
public:
  virtual ~LogTarget() {}

  virtual void log(
      LogLevel level,
      const std::string& message) = 0;

};

#define LOGGER_MAX_LISTENERS 128

class Logger {
public:
  Logger();
  static Logger* get();

  void log(
      LogLevel log_level,
      const std::string& message);

  template <typename... T>
  void log(
      LogLevel log_level,
      const std::string& message,
      T... args) {
    if (log_level >= min_level_) {
      log(log_level, StringUtil::format(message, args...));
    }
  }

  void addTarget(LogTarget* target);
  void setMinimumLogLevel(LogLevel min_level);

  static void logToStderr(
      const std::string& program_name,
      LogLevel min_log_level = LogLevel::kInfo);

  static void logToSyslog(
      const std::string& name,
      LogLevel min_log_level = LogLevel::kInfo);

protected:
  std::atomic<LogLevel> min_level_;
  std::atomic<size_t> max_listener_index_;
  std::atomic<LogTarget*> listeners_[LOGGER_MAX_LISTENERS];
};

/**
 * FATAL: The process is dead
 */
template <typename... T>
void logFatal(const std::string& msg, T... args) {
  Logger::get()->log(LogLevel::kFatal, msg, args...);
}

/**
 * EMERGENCY: Something very bad happened
 */
template <typename... T>
void logEmergency(const std::string& msg, T... args) {
  Logger::get()->log(LogLevel::kEmergency, msg, args...);
}

/**
 * ALERT: Action must be taken immediately
 */
template <typename... T>
void logAlert(const std::string& msg, T... args) {
  Logger::get()->log(LogLevel::kAlert, msg, args...);
}

/**
 * CRITICAL: Action should be taken as soon as possible
 */
template <typename... T>
void logCritical(const std::string& msg, T... args) {
  Logger::get()->log(LogLevel::kCritical, msg, args...);
}

/**
 * ERROR: User-visible Runtime Errors
 */
template <typename... T>
void logError(const std::string& msg, T... args) {
  Logger::get()->log(LogLevel::kError, msg, args...);
}

/**
 * WARNING: Something unexpected happened that should not have happened
 */
template <typename... T>
void logWarning(const std::string& msg, T... args) {
  Logger::get()->log(LogLevel::kWarning, msg, args...);
}

/**
 * NOTICE: Normal but significant condition.
 */
template <typename... T>
void logNotice(const std::string& msg, T... args) {
  Logger::get()->log(LogLevel::kNotice, msg, args...);
}

/**
 * INFO: Informational messages
 */
template <typename... T>
void logInfo(const std::string& msg, T... args) {
  Logger::get()->log(LogLevel::kInfo, msg, args...);
}

/**
 * DEBUG: Debug messages
 */
template <typename... T>
void logDebug(const std::string& msg, T... args) {
  Logger::get()->log(LogLevel::kDebug, msg, args...);
}

/**
 * TRACE: Trace messages
 */
template <typename... T>
void logTrace(const std::string& msg, T... args) {
  Logger::get()->log(LogLevel::kTrace, msg, args...);
}

/**
 * Return the human readable string representation of the provided log leval
 */
const char* logLevelToStr(LogLevel log_level);

/**
 * Return the log level from the human readable string representation. This
 * will raise an exception if no such log level is known
 */
LogLevel strToLogLevel(const std::string& log_level);

