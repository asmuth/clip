/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_UTIL_LOGOUTPUTSTREAM_H
#define _FNORDMETRIC_UTIL_LOGOUTPUTSTREAM_H
#include <fnordmetric/util/logger.h>
#include <fnordmetric/util/outputstream.h>

namespace fnordmetric {
namespace util {

class LogOutputStream : public Logger {
public:
  LogOutputStream(std::unique_ptr<OutputStream> target);
  void log(const LogEntry& log_entry) override;
protected:
  std::unique_ptr<OutputStream> target_;
};

}
}
#endif
