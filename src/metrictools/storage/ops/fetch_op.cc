/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <metrictools/storage/ops/fetch_op.h>

namespace fnordmetric {

FetchStorageOp::FetchStorageOp(
    std::shared_ptr<const GlobalConfig> global_config) noexcept :
    global_config_(global_config) {}

std::ostream& operator<<(std::ostream& out, const FetchStorageOp& op) {
  return out;
}

} // namespace fnordmetric

