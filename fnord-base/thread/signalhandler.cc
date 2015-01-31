/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <signal.h>
#include "fnord-base/thread/signalhandler.h"

namespace fnord {
namespace thread {

void SignalHandler::ignoreSIGHUP() {
  signal(SIGHUP, SIG_IGN);
}

void SignalHandler::ignoreSIGPIPE() {
  signal(SIGPIPE, SIG_IGN);
}

}
}
