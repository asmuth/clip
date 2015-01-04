/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_LOG_LOG_H
#define _FNORD_LOG_LOG_H

namespace fnord {
namespace log {

void log_EMERGENCY(const String& msg) {
  Logger::get()::log(kEmergency, msg);
}

void log_EMERGENCY(const LogTags* tags, const String& msg) {
  Logger::get()::log(kEmergency, tags, msg);
}

template <typename T...>
void logf_EMERGENCY(const String& component, const String& msg, T... args) {
  Logger::get()::logf(kEmergency, msg, args...);
}

template <typename T...>
void logf_EMERGENCY(const LogTags* tags, const String& msg, T... args) {
  Logger::get()::logf(kEmergency, tags, msg, args...);
}

}
}
#endif
