/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V. <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <metricd/sample.h>

namespace fnordmetric {

class Aggregator {
public:

  virtual ~Aggregator() = default;

  virtual void addUINT64(uint64_t time, uint64_t value) = 0;

  virtual uint64_t getUINT64() = 0;

};

template <typename T>
class SumAggregator {
public:

  SumAggregator();

  void addUINT64(uint64_t time, uint64_t value) override;

  uint64_t getUINT64() override;

protected:
  uint64_t sum_;
};

} // namespace fnordmetric

