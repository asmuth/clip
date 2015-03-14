/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnord-base/util/SimpleRateLimit.h"

namespace fnord {
namespace util {

SimpleRateLimit::SimpleRateLimit(
    const Duration& period) :
    period_micros_(period.microseconds()),
    last_micros_(0) {}

bool SimpleRateLimit::check() {
  auto now = WallClock::unixMicros();

  if (now - last_micros_ >= period_micros_) {
    last_micros_ = now;
    return true;
  } else {
    return false;
  }
}

SimpleRateLimitedFn::SimpleRateLimitedFn(
    const Duration& period,
    Function<void ()> fn) :
    limit_(period),
    fn_(fn) {}

void SimpleRateLimitedFn::runMaybe() {
  if (limit_.check()) {
    fn_();
  }
}

void SimpleRateLimitedFn::runForce() {
  fn_();
}

}
}

