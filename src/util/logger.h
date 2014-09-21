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

namespace fnordmetric {
namespace util {

class Logger {
public:
  typedef std::vector<std::pair<std::string, std::string>> LogEntry;

  Logger() {}
  virtual ~Logger() {}

  void log(const std::string& severity, const std::string& message);

  virtual void log(const LogEntry& log_entry);

};


}
}
#endif
