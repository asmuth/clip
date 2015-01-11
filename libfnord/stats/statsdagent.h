/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_STATS_STATSDAGENT_H_
#define _FNORD_STATS_STATSDAGENT_H_
#include "fnord/base/stdtypes.h"
#include "fnord/stats/stat.h"
#include "fnord/stats/statsrepository.h"

namespace fnord {
namespace stats {

class StatsdAgent {
public:
  StatsdAgent();
  StatsdAgent(StatsRepository* stats_repo);

protected:
  StatsRepository* stats_repo_;
};


}
}
#endif
