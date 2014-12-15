/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnord/logging/loglevel.h"
#include "fnord/base/stringutil.h"

namespace fnord {
namespace log {

const char* logLevelToStr(LogLevel log_level) {
  switch (log_level) {
    case 9000: return "EMERGENCY";
    case 8000: return "ALERT";
    case 7000: return "CRITICAL";
    case 6000: return "ERROR";
    case 5000: return "WARNING";
    case 4000: return "NOTICE";
    case 3000: return "INFO";
    case 2000: return "DEBUG";
    case 1000: return "TRACE";
    default: return "CUSTOM"; // FIXPAUL
  }
}

}
}

