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
#include <atomic>
#include "fnord/base/datetime.h"
#include "fnord/base/stdtypes.h"

namespace fnord {
class LogTarget;
class Logger;

typedef int LogLevel;

/**
 * EMERGENCY: Something very bad happened
 */
namespace loglevel {
const LogLevel kEmergency = 9000;
}

template <typename... T>
void logEmergency(const String& component, const String& msg, T... args);

template <typename... T>
void logEmergency(
    const String& component,
    const std::exception& e,
    const String& msg,
    T... args);

/**
 * ALERT: Action must be taken immediately
 */
namespace loglevel {
const LogLevel kAlert = 8000;
}

template <typename... T>
void logAlert(const String& component, const String& msg, T... args);

template <typename... T>
void logAlert(
    const String& component,
    const std::exception& e,
    const String& msg,
    T... args);

/**
 * CRITICAL: Action should be taken as soon as possible
 */
namespace loglevel {
const LogLevel kCritical = 7000;
}

template <typename... T>
void logCritical(const String& component, const String& msg, T... args);

template <typename... T>
void logCritical(
    const String& component,
    const std::exception& e,
    const String& msg,
    T... args);

/**
 * ERROR: User-visible Runtime Errors
 */
namespace loglevel {
const LogLevel kError = 6000;
}

template <typename... T>
void logError(const String& component, const String& msg, T... args);

template <typename... T>
void logError(
    const String& component,
    const std::exception& e,
    const String& msg,
    T... args);

/**
 * WARNING: Something unexpected happened that should not have happened
 */
namespace loglevel {
const LogLevel kWarning = 5000;
}

template <typename... T>
void logWarning(const String& component, const String& msg, T... args);

template <typename... T>
void logWarning(
    const String& component,
    const std::exception& e,
    const String& msg,
    T... args);

/**
 * NOTICE: Normal but significant condition.
 */
namespace loglevel {
const LogLevel kNotice = 4000;
}

template <typename... T>
void logNotice(const String& component, const String& msg, T... args);

template <typename... T>
void logNotice(
    const String& component,
    const std::exception& e,
    const String& msg,
    T... args);

/**
 * INFO: Informational messages
 */
namespace loglevel {
const LogLevel kInfo = 3000;
}

template <typename... T>
void logInfo(const String& component, const String& msg, T... args);

template <typename... T>
void logInfo(
    const String& component,
    const std::exception& e,
    const String& msg,
    T... args);

/**
 * DEBUG: Debug messages
 */
namespace loglevel {
const LogLevel kDebug = 2000;
}

template <typename... T>
void logDebug(const String& component, const String& msg, T... args);

template <typename... T>
void logDebug(
    const String& component,
    const std::exception& e,
    const String& msg,
    T... args);

/**
 * TRACE: Trace messages
 */
namespace loglevel {
const LogLevel kTrace = 1000;
}

template <typename... T>
void logTrace(const String& component, const String& msg, T... args);

template <typename... T>
void logTrace(
    const String& component,
    const std::exception& e,
    const String& msg,
    T... args);

/**
 * Return the human readable string representation of the provided log leval
 */
const char* logLevelToStr(LogLevel log_level);

/**
 * Return the log level from the human readable string representation. This
 * will raise an exception if no such log level is known
 */
LogLevel strToLogLevel(const char* log_level);

#ifndef FNORD_LOGGER_MAX_LISTENERS
#define FNORD_LOGGER_MAX_LISTENERS 64
#endif

class Logger {
public:
  Logger();
  static Logger* get();

  void log(
      LogLevel log_level,
      const String& component,
      const String& message);

  template <typename... T>
  void log(
      LogLevel log_level,
      const String& component,
      const String& message,
      T... args);

  void logException(
      LogLevel log_level,
      const String& component,
      const std::exception& exception);

  void logException(
      LogLevel log_level,
      const String& component,
      const String& message,
      const std::exception& exception);

  void addTarget(LogTarget* target);
  void setMinimumLogLevel(LogLevel min_level);

protected:

  void logInternal(
      LogLevel log_level,
      const String& message);

  std::atomic<LogLevel> min_level_;
  std::atomic<size_t> max_listener_index_;
  std::atomic<LogTarget*> listeners_[FNORD_LOGGER_MAX_LISTENERS];
};

class LogTarget {
public:
  virtual ~LogTarget() {}

  virtual void log(
      LogLevel level,
      const String& component,
      const String& message) = 0;

};

}

#endif
