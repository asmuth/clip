/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_STATS_STAT_H
#define _FNORD_STATS_STAT_H

namespace fnord {
namespace stats {

template <typename ValueType, typename... DimensionTypes>
class Stat {
public:

  template <typename... DimensionTitleTypes>
  CounterStat();

  virtual ~CounterStat {}

  virtual void exportAll(OutputStream* out) const = 0;

protected:

  T* lookup(DimensionTypes... dimensions);

  void forEach(
      std::function<void (const T& val)> callback,
      DimensionTypes... dimensions);

private:
  std::unordered_map<std::tuple<DimensionTypes...>, ValueType> values_;
  mutable std::mutex mutex_;
};

}
}

#endif
