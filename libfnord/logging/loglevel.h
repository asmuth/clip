/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_LOG_LOGLEVEL_H
#define _FNORD_LOG_LOGLEVEL_H

namespace fnord {
namespace log {

typedef int LogLevel;

/**
 * EMERGENCY: Something very bad happened
 */
const LogLevel kEmergency = 9000;

/**
 * ALERT: Action must be taken immediately
 */
const LogLevel kAlert = 8000;

/**
 * CRITICAL: Action should be taken as soon as possible
 */
const LogLevel kCritical = 7000;

/**
 * ERROR: User-visible Runtime Errors
 */
const LogLevel kError = 6000;

/**
 * WARNING: Something unexpected happened that should not have happened
 */
const LogLevel kWarning = 5000;

/**
 * NOTICE: Normal but significant condition.
 */
const LogLevel kNotice = 4000;

/**
 * INFO: Informational messages
 */
const LogLevel kInfo = 3000;

/**
 * DEBUG: Debug messages
 */
const LogLevel kDebug = 2000;

/**
 * TRACE: Trace messages
 */
const LogLevel kTrace = 1000;


/**
 * Return the human readable string representation of the provided log leval
 */
const char* logLevelToStr(LogLevel log_level);

/**
 * Return the log level from the human readable string representation. This
 * will raise an exception if no such log level is known
 */
LogLevel strToLogLevel(const char* log_level);


}
}
#endif
