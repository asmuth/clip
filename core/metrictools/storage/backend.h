/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <memory>
#include <vector>
#include <metrictools/util/return_code.h>
#include <metrictools/storage/ops/insert_op.h>
#include <metrictools/storage/ops/fetch_op.h>
#include <libtransport/uri/uri.h>

namespace fnordmetric {

class Backend {
public:

  static ReturnCode openBackend(
      const URI& backend_uri,
      std::unique_ptr<Backend>* backend);

  virtual ~Backend() = default;

  virtual ReturnCode performOperation(InsertStorageOp* op) = 0;

  virtual ReturnCode performOperation(FetchStorageOp* op) = 0;

  virtual void shutdown() {}

};

} // namespace fnordmetric

