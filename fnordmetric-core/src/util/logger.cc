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
#include <fnordmetric/util/runtimeexception.h>
#include <sstream>
#include <stdarg.h>

using fnordmetric::util::RuntimeException;

namespace fnord {
namespace util {

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

void Logger::exception(
    const std::string& severity,
    const std::string& message,
    const std::exception& exception) {
  LogEntry entry;
  entry.append("__severity__", severity);
  entry.append("__message__", message);

  try {
    auto rte = dynamic_cast<const RuntimeException&>(exception);
    entry.append("exception", rte.getTypeName());
    entry.printf("exception", "    in %s", rte.method().c_str());
    entry.printf("exception", "    in %s:%i", rte.file().c_str(), rte.line());

    std::stringstream ss(rte.getMessage());
    std::string line;
    while (std::getline(ss, line, '\n')) {
      entry.append("exception", "message: " + line);
    }
  } catch (const std::exception& e) {
    entry.append("exception", "std::exception: <unknown exception>");
  }

  log(entry);
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

}
}
