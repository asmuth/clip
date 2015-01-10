/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_STATS_STATSREPOSITORY_H
#define _FNORD_STATS_STATSREPOSITORY_H
#include <mutex>
#include <stdlib.h>
#include <stdint.h>

namespace fnord {
namespace stats {

class StatsRepository {
public:
  static StatsRepository* get();

protected:
  std::mutex mutex_;
};

}
}
#endif
