/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/util/logger.h>
#include <fnordmetric/util/wallclock.h>

namespace fnordmetric {
namespace util {

void Logger::log(const std::string& severity, const std::string& message) {
  LogEntry entry;
  entry.append("__severity__", severity);
  entry.append("__message__", message);
  log(entry);
}

LogEntry::LogEntry() : time_(WallClock::now()) {}

void LogEntry::append(const std::string& key, const std::string& value) {
  lines_.emplace_back(key, value);
}

const std::vector<std::pair<std::string, std::string>>& LogEntry::lines()
    const {
  return lines_;
}

const DateTime& LogEntry::time() const {
  return time_;
}

}
}
