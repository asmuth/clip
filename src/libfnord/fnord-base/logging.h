/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_UTIL_LOGGING_H
#define _FNORDMETRIC_UTIL_LOGGING_H

#include "fnord-base/logging/loglevel.h"
#include "fnord-base/logging/logger.h"

namespace fnord {

/**
 * EMERGENCY: Something very bad happened
 */
template <typename... T>
void logEmergency(const String& component, const String& msg, T... args) {
  Logger::get()->log(LogLevel::kEmergency, component, msg, args...);
}

template <typename... T>
void logEmergency(
    const String& component,
    const std::exception& e,
    const String& msg,
    T... args) {
  Logger::get()->logException(LogLevel::kEmergency, component, e, msg, args...);
}

/**
 * ALERT: Action must be taken immediately
 */
template <typename... T>
void logAlert(const String& component, const String& msg, T... args) {
  Logger::get()->log(LogLevel::kAlert, component, msg, args...);
}

template <typename... T>
void logAlert(
    const String& component,
    const std::exception& e,
    const String& msg,
    T... args) {
  Logger::get()->logException(LogLevel::kAlert, component, e, msg, args...);
}

/**
 * CRITICAL: Action should be taken as soon as possible
 */
template <typename... T>
void logCritical(const String& component, const String& msg, T... args) {
  Logger::get()->log(LogLevel::kCritical, component, msg, args...);
}

template <typename... T>
void logCritical(
    const String& component,
    const std::exception& e,
    const String& msg,
    T... args) {
  Logger::get()->logException(LogLevel::kCritical, component, e, msg, args...);
}

/**
 * ERROR: User-visible Runtime Errors
 */
template <typename... T>
void logError(const String& component, const String& msg, T... args) {
  Logger::get()->log(LogLevel::kError, component, msg, args...);
}

template <typename... T>
void logError(
    const String& component,
    const std::exception& e,
    const String& msg,
    T... args) {
  Logger::get()->logException(LogLevel::kError, component, e, msg, args...);
}

/**
 * WARNING: Something unexpected happened that should not have happened
 */
template <typename... T>
void logWarning(const String& component, const String& msg, T... args) {
  Logger::get()->log(LogLevel::kWarning, component, msg, args...);
}

template <typename... T>
void logWarning(
    const String& component,
    const std::exception& e,
    const String& msg,
    T... args) {
  Logger::get()->logException(LogLevel::kWarning, component, e, msg, args...);
}

/**
 * NOTICE: Normal but significant condition.
 */
template <typename... T>
void logNotice(const String& component, const String& msg, T... args) {
  Logger::get()->log(LogLevel::kNotice, component, msg, args...);
}

template <typename... T>
void logNotice(
    const String& component,
    const std::exception& e,
    const String& msg,
    T... args) {
  Logger::get()->logException(LogLevel::kNotice, component, e, msg, args...);
}

/**
 * INFO: Informational messages
 */
template <typename... T>
void logInfo(const String& component, const String& msg, T... args) {
  Logger::get()->log(LogLevel::kInfo, component, msg, args...);
}

template <typename... T>
void logInfo(
    const String& component,
    const std::exception& e,
    const String& msg,
    T... args) {
  Logger::get()->logException(LogLevel::kInfo, component, e, msg, args...);
}

/**
 * DEBUG: Debug messages
 */
template <typename... T>
void logDebug(const String& component, const String& msg, T... args) {
  Logger::get()->log(LogLevel::kDebug, component, msg, args...);
}

template <typename... T>
void logDebug(
    const String& component,
    const std::exception& e,
    const String& msg,
    T... args) {
  Logger::get()->logException(LogLevel::kDebug, component, e, msg, args...);
}

/**
 * TRACE: Trace messages
 */
template <typename... T>
void logTrace(const String& component, const String& msg, T... args) {
  Logger::get()->log(LogLevel::kTrace, component, msg, args...);
}

template <typename... T>
void logTrace(
    const String& component,
    const std::exception& e,
    const String& msg,
    T... args) {
  Logger::get()->logException(LogLevel::kTrace, component, e, msg, args...);
}

/**
 * Return the human readable string representation of the provided log leval
 */
const char* logLevelToStr(LogLevel log_level);

/**
 * Return the log level from the human readable string representation. This
 * will raise an exception if no such log level is known
 */
LogLevel strToLogLevel(const String& log_level);

}

#endif
