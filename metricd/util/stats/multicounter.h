/**
 * Copyright (c) 2016 zScale Technology GmbH <legal@zscale.io>
 * Authors:
 *   - Paul Asmuth <paul@zscale.io>
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License ("the license") as
 * published by the Free Software Foundation, either version 3 of the License,
 * or any later version.
 *
 * In accordance with Section 7(e) of the license, the licensing of the Program
 * under the license does not imply a trademark license. Therefore any rights,
 * title and interest in our trademarks remain entirely with us.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the license for more details.
 *
 * You can be released from the requirements of the license by purchasing a
 * commercial license. Buying such a license is mandatory as soon as you develop
 * commercial activities involving this program without disclosing the source
 * code of your own applications
 */
#ifndef _STX_STATS_MULTICOUNTER_H
#define _STX_STATS_MULTICOUNTER_H
#include <stdlib.h>
#include <stdint.h>
#include "fnordmetric/util/time.h"
#include "fnordmetric/util/hash.h"
#include "fnordmetric/util/stats/stat.h"

namespace stats {

template <typename ValueType, typename... LabelTypes>
class MultiCounterStat : public stats::Stat {
public:
  typedef std::tuple<LabelTypes...> LabelValuesType;

  template <typename... LabelNameTypes>
  MultiCounterStat(LabelNameTypes... label_names);

  void increment(ValueType value);
  void set(ValueType value);

  void exportAll(const String& path, StatsSink* sink) const override;

protected:
  std::unordered_map<LabelValuesType, ValueType, fnordmetric::hash<LabelValuesType>> values_;
  mutable std::mutex mutex_;
};

template <typename ValueType, typename... LabelTypes>
class MultiCounter : public StatRef {
public:

  template <typename... LabelNameTypes>
  MultiCounter(LabelNameTypes... label_names);

  void increment(ValueType value);
  void set(ValueType value);

  RefPtr<Stat> getStat() const override;

protected:
  RefPtr<MultiCounterStat<ValueType, LabelTypes...>> stat_;
};

}

#include "multicounter_impl.h"
#endif
