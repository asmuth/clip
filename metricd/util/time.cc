/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V.
 *   Copyright (c) 2016 Laura Schlimmer, FnordCorp B.V.
 *   Copyright (c) 2016 Christian Parpart
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <iostream>
#include <string>
#include <ctime>
#include <sys/time.h>
#include <sys/types.h>
#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#endif
#include "time.h"
#include "stringutil.h"

UnixTime WallClock::now() {
  return UnixTime(WallClock::getUnixMicros());
}

uint64_t WallClock::unixSeconds() {
  struct timeval tv;

  gettimeofday(&tv, NULL);
  return tv.tv_sec;
}

uint64_t WallClock::getUnixMillis() {
  return unixMillis();
}

uint64_t WallClock::unixMillis() {
  struct timeval tv;

  gettimeofday(&tv, NULL);
  return tv.tv_sec * 1000llu + tv.tv_usec / 1000llu;
}

uint64_t WallClock::getUnixMicros() {
  return unixMicros();
}

uint64_t WallClock::unixMicros() {
  struct timeval tv;

  gettimeofday(&tv, NULL);
  return tv.tv_sec * 1000000llu + tv.tv_usec;
}

uint64_t MonotonicClock::now() {
#ifdef __MACH__
  clock_serv_t cclock;
  mach_timespec_t mts;
  host_get_clock_service(mach_host_self(), SYSTEM_CLOCK, &cclock);
  clock_get_time(cclock, &mts);
  mach_port_deallocate(mach_task_self(), cclock);
  return std::uint64_t(mts.tv_sec) * 1000000 + mts.tv_nsec / 1000;
#else
  timespec ts;
  if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
    std::cerr << "clock_gettime(CLOCK_MONOTONIC) failed\n";
    abort();
  } else {
    return std::uint64_t(ts.tv_sec) * 1000000 + ts.tv_nsec / 1000;
  }
#endif
}

UnixTime::UnixTime() :
    utc_micros_(WallClock::unixMicros()) {}

UnixTime& UnixTime::operator=(const UnixTime& other) {
  utc_micros_ = other.utc_micros_;
  return *this;
}

UnixTime UnixTime::now() {
  return UnixTime(WallClock::unixMicros());
}

UnixTime UnixTime::daysFromNow(double days) {
  return UnixTime(WallClock::unixMicros() + (days * kMicrosPerDay));
}

std::string UnixTime::toString(const char* fmt) const {
  struct tm tm;
  time_t tt = utc_micros_ / 1000000;
  gmtime_r(&tt, &tm); // FIXPAUL

  char buf[256]; // FIXPAUL
  buf[0] = 0;
  strftime(buf, sizeof(buf), fmt, &tm);

  return std::string(buf);
}

template <>
std::string StringUtil::toString(UnixTime value) {
  return value.toString();
}

UnixTime std::numeric_limits<UnixTime>::min() {
  return UnixTime::epoch();
}

UnixTime std::numeric_limits<UnixTime>::max() {
  return UnixTime(std::numeric_limits<uint64_t>::max());
}
