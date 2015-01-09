/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnord/base/logging.h"
#include "fnord/base/logging/logoutputstream.h"
#include "fnord/base/stringutil.h"
#include "fnord/base/wallclock.h"

using fnord::OutputStream;

namespace fnord {

LogOutputStream::LogOutputStream(
    std::unique_ptr<OutputStream> target) :
    target_(std::move(target)) {}

void LogOutputStream::log(
    LogLevel level,
    const String& component,
    const String& message) {
  const auto prefix = StringUtil::format(
      "$0 $1 [$2] ",
      WallClock::now().toString("%Y-%m-%d %H:%M:%S"),
      logLevelToStr(level),
      component);

  std::string lines = prefix + message;
  StringUtil::replaceAll(&lines, "\n", "\n" + prefix);
  lines.append("\n");

  ScopedLock<std::mutex> lk(target_->mutex_);
  target_->write(lines);
}

}
