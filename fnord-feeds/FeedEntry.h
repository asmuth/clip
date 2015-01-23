/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_FEEDS_LOGSTREAMENTRY_H
#define _FNORD_FEEDS_LOGSTREAMENTRY_H
#include <stdlib.h>
#include <string>
#include "fnord/base/datetime.h"

namespace fnord {
namespace feeds {

typedef uint64_t FeedOffset;

struct FeedEntry {
  uint64_t offset;
  uint64_t next_offset;
  DateTime time;
  std::string data;

  template <typename T>
  static void reflect(T* meta) {
    meta->prop(&FeedEntry::offset, 1, "offset", false);
    meta->prop(&FeedEntry::next_offset, 2, "next_offset", false);
    meta->prop(&FeedEntry::data, 3, "data", false);
  }
};

} // namespace logstream_service
} // namespace fnord
#endif
