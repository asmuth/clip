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

#include "fnord-base/io/outputstream.h"
#include "fnord-base/logging/loglevel.h"
#include "fnord-base/logging/logtarget.h"
#include "fnord-base/stdtypes.h"

namespace fnord {

class LogOutputStream : public LogTarget {
public:
  LogOutputStream(std::unique_ptr<OutputStream> target);

  void log(
      LogLevel level,
      const String& component,
      const String& message) override;

protected:
  ScopedPtr<OutputStream> target_;
};

}
#endif
