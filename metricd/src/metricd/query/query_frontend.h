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
#include <atomic>
#include <memory>
#include <string>
#include "metricd/metric.h"
#include "metricd/metric_service.h"
#include "metricd/query/query_options.h"
#include "metricd/query/data_frame.h"
#include "metricd/query/ops/fetch_timeseries.h"
#include <libtransport/json/json.h>
#include <libtransport/json/json_writer.h>
#include <libtransport/json/json_object.h>

namespace fnordmetric {

namespace json = libtransport::json;

class QueryFrontend {
public:

  QueryFrontend(MetricService* metric_service);

  ReturnCode fetchSummaryJSON(
      const json::JSONObject* req,
      json::JSONWriter* res);

protected:
  MetricService* metric_service_;
};

} // namespace fnordmetric

