/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V. <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <assert.h>
#include <string.h>
#include <metricd/summarize.h>

namespace fnordmetric {

SumGroupSummary::SumGroupSummary(tval_type val_type) : acc_(val_type) {}

void SumGroupSummary::addValue(
    tval_type value_type,
    const void* value,
    size_t value_len) {
  assert(value_type == acc_.val.type);
  assert(value_len == acc_.val.len);
  tval_add(acc_.val.type, acc_.val.data, acc_.val.len, value, value_len);
}

void SumGroupSummary::getValue(
    tval_type value_type,
    void* value,
    size_t value_len) {
  assert(value_type == acc_.val.type);
  assert(value_len == acc_.val.len);
  memcpy(value, acc_.val.data, acc_.val.len);
}

void SumGroupSummary::reset() {
  tval_zero(acc_.val.type, acc_.val.data, acc_.val.len);
}

} // namespace fnordmetric

