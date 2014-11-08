/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/util/logoutputstream.h>

using fnordmetric::util::OutputStream;

namespace fnord {
namespace util {

LogOutputStream::LogOutputStream(
    std::unique_ptr<OutputStream> target) :
    target_(std::move(target)) {}

void LogOutputStream::log(const LogEntry& log_entry) {
  std::string severity = "INFO";
  std::vector<std::string> lines;
  for (const auto& line : log_entry.lines()) {
    if (line.first == "__severity__") {
      severity = line.second;
      continue;
    }

    if (line.first == "__message__") {
      lines.emplace_back(line.second);
      continue;
    }

    lines.emplace_back("-- " + line.first + ": " + line.second);
  }

  std::string prefix;
  prefix.append(log_entry.time().toString("%Y-%m-%d %H:%M:%S"));
  prefix.append(" [" + severity + "] ");

  std::string msg;
  for (const auto& line : lines) {
    msg.append(prefix);
    msg.append(line);
    msg.append("\n");
  }

  target_->mutex_.lock();
  try {
    target_->write(msg.c_str(), msg.size());
  } catch (const std::exception& e) {
    target_->mutex_.unlock();
    throw e;
  }

  target_->mutex_.unlock();
}

}
}
