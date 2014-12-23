/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_STATS_COUNTERSTAT_H
#define _FNORD_STATS_COUNTERSTAT_H

namespace fnord {
namespace stats {

/**
 * CounterStat<uint64_t, std::string> http_req_stat("http_reqs", "status_code");
 * http_req_stat.incr(1, "200");
 */
template <typename ValueType, typename... DimensionTypes>
class CounterStat : public Stat<ValueType, DimensionTypes...> {
public:
  static_assert(
      std::is_arithmetic<ValueType>::value,
      "ValueType must be arithmetic");

  template <typename... DimensionTitleTypes>
  CounterStat();

  void incr(T value, DimensionTypes... dimensions);
  void desc(T value, DimensionTypes... dimensions);
  T value(DimensionTypes... dimensions) const;

  void exportAll(io::OutputStream* out) const override;

};

}
}

#endif
