/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <libpaint/util/daemonize.h>

namespace fnordmetric {

ReturnCode daemonize() {
#if defined(_WIN32)
#error "Application::daemonize() not yet implemented for windows"

#elif defined(__APPLE__) && defined(__MACH__)
  // FIXME: deprecated on OSX
  if (::daemon(true /*no chdir*/, true /*no close*/) < 0) {
    return ReturnCode::error("INIT_FAIL", "daemon() failed");
  }

#elif defined(__linux__) || defined(__linux) || defined(linux) || defined(__gnu_linux__)
  if (::daemon(true /*no chdir*/, true /*no close*/) < 0) {
    return ReturnCode::error("INIT_FAIL", "daemon() failed");
  }

#else
#error Unsupported OS
#endif
  return ReturnCode::success();
}


} // namespace fnordmetric

