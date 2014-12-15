/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnord/base/stringutil.h"
#include "fnord/base/wallclock.h"
#include "fnord/logging/logoutputstream.h"

using fnord::io::OutputStream;

namespace fnord {
namespace log {

LogOutputStream::LogOutputStream(
    std::unique_ptr<OutputStream> target) :
    target_(std::move(target)) {}

void LogOutputStream::log(
    LogLevel level,
    const LogTags* tags,
    const std::string& message) {
  const auto& severity = logLevelToStr(level);
  const auto time = WallClock::now();

  std::string line = StringUtil::format(
      "$0 [$1] $2\n",
      time.toString("%Y-%m-%d %H:%M:%S"),
      severity,
      message);

  std::lock_guard<std::mutex> lock_holder(target_->mutex_);
  target_->write(line);
}

}
}
