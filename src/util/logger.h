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
#include <fnordmetric/util/datetime.h>
#include <string>
#include <vector>

namespace fnordmetric {
namespace util {

class LogEntry {
public:
  LogEntry();
  void append(const std::string& key, const std::string& value);
  void printf(const std::string& key, const std::string& value, ...);
  const std::vector<std::pair<std::string, std::string>>& lines() const;
  const DateTime& time() const;
protected:
  DateTime time_;
  std::vector<std::pair<std::string, std::string>> lines_;
};

class Logger {
public:
  Logger() {}
  virtual ~Logger() {}

  void log(const std::string& severity, const std::string& message);

  void exception(
      const std::string& severity,
      const std::string& message,
      const std::exception& exception);

  virtual void log(const LogEntry& log_entry) = 0;

};

}
}
#endif
