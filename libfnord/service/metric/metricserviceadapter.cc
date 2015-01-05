/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnord/json/jsonrpcrequest.h"
#include "fnord/json/jsonrpcresponse.h"
#include "fnord/service/metric/metricservice.h"
#include "fnord/service/metric/metricserviceadapter.h"

namespace fnord {
namespace metric_service {

void MetricServiceAdapter::registerJSONRPC(
    MetricService* metric_service,
    json::JSONRPC* rpc) {

  rpc->registerMethod("MetricService.listMetrics", [metric_service] (
      json::JSONRPCRequest* req,
      json::JSONRPCResponse* res) {
    const auto& metric_list = metric_service->listMetrics();

    res->success([&metric_list] (json::JSONOutputStream* json) {
      json->beginArray();

      int i = 0;
      for (const auto& metric : metric_list) {
        if (i++ > 0) { json->addComma(); }
        json->beginObject();

        json->addObjectEntry("key");
        json->addString(metric->key());
        json->addComma();

        json->addObjectEntry("total_bytes");
        json->addValue(metric->totalBytes());
        json->addComma();

        json->addObjectEntry("last_insert");
        json->addValue(static_cast<uint64_t>(metric->lastInsertTime()));
        json->addComma();

        json->addObjectEntry("labels");
        json->beginArray();
        auto labels = metric->labels();
        for (auto cur = labels.begin(); cur != labels.end(); ++cur) {
          if (cur != labels.begin()) {
            json->addComma();
          }
          json->addString(*cur);
        }
        json->endArray();

        json->endObject();
      }

      json->endArray();
    });
  });

  rpc->registerMethod("MetricService.insertSample", [metric_service] (
      json::JSONRPCRequest* req,
      json::JSONRPCResponse* res) {
    auto p_metric = req->body().get("/params/metric");
    auto p_value = req->body().getAs<double>("/params/value");

    std::vector<std::pair<std::string, std::string>> labels;
    req->body().forEach("/params/labels", [&] (
        const json::JSONPointer& path) -> bool {
      auto label_key = path.head();
      auto label_value = req->body().get(path);

      labels.emplace_back(std::make_pair(label_key, label_value));
      return true;
    });

    metric_service->insertSample(p_metric, p_value, labels);
    res->successAndReturn(true);
  });

  rpc->registerMethod("MetricService.scanSamples", [metric_service] (
      json::JSONRPCRequest* req,
      json::JSONRPCResponse* res) {
    const auto& doc = req->body();

    auto p_metric = doc.get("/params/metric");
    auto p_from = doc.getAs<DateTime>("/params/from", DateTime::epoch());
    auto p_until = doc.getAs<DateTime>("/params/until", DateTime::now());

    res->success([&] (json::JSONOutputStream* json) {
      int i = 0;
      auto render_sample = [&json, &i] (Sample* sample) -> bool {
        if (i++ > 0) { json->addComma(); }
        json->beginObject();

        json->addObjectEntry("time");
        json->addValue<uint64_t>(static_cast<uint64_t>(sample->time()));
        json->addComma();

        json->addObjectEntry("value");
        json->addValue<double>(sample->value());
        json->addComma();

        json->addObjectEntry("labels");
        json->beginObject();
        auto labels = sample->labels();
        for (int n = 0; n < labels.size(); n++) {
          if (n > 0) {
            json->addComma();
          }

          json->addObjectEntry(labels[n].first);
          json->addString(labels[n].second);
        }
        json->endObject();

        json->endObject();
        return true;
      };

      json->beginArray();

      metric_service->scanSamples(
          p_metric,
          p_from,
          p_until,
          render_sample);

      json->endArray();
    });
  });
}

} // namespace metric_service
} // namsepace fnord

