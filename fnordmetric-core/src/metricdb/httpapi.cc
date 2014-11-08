/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/metricdb/httpapi.h>
#include <fnordmetric/query/queryservice.h>
#include <fnordmetric/metricdb/metricrepository.h>
#include <fnordmetric/metricdb/metrictablerepository.h>
#include <fnordmetric/util/stringutil.h>
#include <fnordmetric/util/inputstream.h>
#include <fnordmetric/util/outputstream.h>
#include <xzero/http/HttpStatus.h>
#include <xzero/http/HttpRequest.h>
#include <xzero/http/HttpResponse.h>
#include <xzero/http/HttpOutput.h>

namespace fnordmetric {
namespace metricdb {

class BufferInputStream : public util::InputStream { // {{{
 public:
  BufferInputStream();
  BufferInputStream(xzero::Buffer&& data);

  bool readNextByte(char* target) override;

 private:
  xzero::Buffer buffer_;
  size_t readOffset_;
};

BufferInputStream::BufferInputStream()
    : util::InputStream(),
      buffer_(),
      readOffset_(0) {
}

BufferInputStream::BufferInputStream(xzero::Buffer&& data)
    : util::InputStream(),
      buffer_(std::move(data)),
      readOffset_(0) {
}

bool BufferInputStream::readNextByte(char* target) {
  if (readOffset_ < buffer_.size()) {
    *target = buffer_[readOffset_++];
    return true;
  }

  return false;
}

std::shared_ptr<util::InputStream> createInputStream(xzero::HttpRequest* request) {
  xzero::Buffer body;
  request->input()->read(&body);
  return std::shared_ptr<util::InputStream>(new BufferInputStream(std::move(body)));
}
// }}}
class BufferOutputStream : public util::OutputStream { // {{{
 public:
  BufferOutputStream();

  size_t write(const char* data, size_t size) override;

  void clear() { buffer_.clear(); }
  size_t size() const noexcept { return buffer_.size(); }
  const xzero::Buffer& buffer() const noexcept { return buffer_; }
  xzero::Buffer&& take() { return std::move(buffer_); }

 private:
  xzero::Buffer buffer_;
};

BufferOutputStream::BufferOutputStream()
    : buffer_() {
}

size_t BufferOutputStream::write(const char* data, size_t size) {
  buffer_.push_back(data, size);
  return size;
}
// }}}

static const char kMetricsUrl[] = "/metrics";
static const char kMetricsUrlPrefix[] = "/metrics/";
static const char kQueryUrl[] = "/query";
static const char kLabelParamPrefix[] = "label[";

HTTPAPI::HTTPAPI(MetricRepository* metric_repo) : metric_repo_(metric_repo) {}

bool HTTPAPI::handleRequest(
    xzero::HttpRequest* request,
    xzero::HttpResponse* response) {
  util::URI uri(request->unparsedUri());
  std::string path = request->path();
  fnord::util::StringUtil::stripTrailingSlashes(&path);

  // PATH: ^/metrics/?$
  if (path == kMetricsUrl) {
    switch (request->method()) {
      case xzero::HttpMethod::GET:
        renderMetricList(request, response, &uri);
        return true;
      case xzero::HttpMethod::POST:
        insertSample(request, response, &uri);
        return true;
      default:
        return false;
    }
  }

  // PATH: ^/metrics/.*
  if (path.compare(0, sizeof(kMetricsUrlPrefix) - 1, kMetricsUrlPrefix) == 0) {
    // PATH: ^/metrics/(.*)$
    switch (request->method()) {
      case xzero::HttpMethod::GET:
        renderMetricSampleScan(request, response, &uri);
        return true;
      default:
        return false;
    }
  }

  // PATH: ^/query/?*
  if (path == kQueryUrl) {
    switch (request->method()) {
      case xzero::HttpMethod::GET:
      case xzero::HttpMethod::POST:
        executeQuery(request, response, &uri);
        return true;
      default:
        return false;
    }
    return true;
  }

  return false;
}

void HTTPAPI::renderMetricList(
    xzero::HttpRequest* request,
    xzero::HttpResponse* response,
    util::URI* uri) {
  std::shared_ptr<BufferOutputStream> output_stream(new BufferOutputStream());
  util::JSONOutputStream json(output_stream);

  json.beginObject();
  json.addObjectEntry("metrics");
  json.beginArray();

  int i = 0;
  for (const auto& metric : metric_repo_->listMetrics()) {
    if (i++ > 0) { json.addComma(); }
    renderMetricJSON(metric, &json);
  }

  json.endArray();
  json.endObject();

  response->setStatus(xzero::HttpStatus::Ok);
  response->addHeader("Content-Type", "application/json; charset=utf-8");
  response->setContentLength(output_stream->size());
  response->output()->write(
      std::move(output_stream->take()),
      std::bind(&xzero::HttpResponse::completed, response));
}

void HTTPAPI::insertSample(
    xzero::HttpRequest* request,
    xzero::HttpResponse* response,
    util::URI* uri) {
  auto params = uri->queryParams();

  std::string metric_key;
  if (!util::URI::getParam(params, "metric", &metric_key)) {
    response->sendError(
        xzero::HttpStatus::BadRequest,
        "error: invalid metric key: " + metric_key);
    return;
  }

  std::string value_str;
  if (!util::URI::getParam(params, "value", &value_str)) {
    response->sendError(
        xzero::HttpStatus::BadRequest,
        "error: missing ?value=... parameter");
    return;
  }

  std::vector<std::pair<std::string, std::string>> labels;
  for (const auto& param : params) {
    const auto& key = param.first;
    const auto& value = param.second;

    if (key.compare(0, sizeof(kLabelParamPrefix) - 1, kLabelParamPrefix) == 0 &&
        key.back() == ']') {
      auto label_key = key.substr(
          sizeof(kLabelParamPrefix) - 1,
          key.size() - sizeof(kLabelParamPrefix));

      labels.emplace_back(label_key, value);
    }
  }

  double sample_value;
  try {
    sample_value = std::stod(value_str);
  } catch (std::exception& e) {
    response->sendError(
        xzero::HttpStatus::BadRequest,
        "error: invalid value: " + value_str);
    return;
  }

  auto metric = metric_repo_->findOrCreateMetric(metric_key);
  metric->insertSample(sample_value, labels);

  response->setStatus(xzero::HttpStatus::Created);
  response->completed();
}

void HTTPAPI::renderMetricSampleScan(
    xzero::HttpRequest* request,
    xzero::HttpResponse* response,
    util::URI* uri) {
  auto metric_key = uri->path().substr(sizeof(kMetricsUrlPrefix) - 1);
  if (metric_key.size() < 3) {
    response->sendError(
        xzero::HttpStatus::BadRequest,
        "error: invalid metric key: " + metric_key);
    return;
  }

  auto metric = metric_repo_->findMetric(metric_key);
  if (metric == nullptr) {
    response->sendError(
        xzero::HttpStatus::NotFound,
        "metric not found: " + metric_key);
    return;
  }

  std::shared_ptr<BufferOutputStream> output_stream(new BufferOutputStream());
  util::JSONOutputStream json(output_stream);

  json.beginObject();

  json.addObjectEntry("metric");
  renderMetricJSON(metric, &json);
  json.addComma();

  json.addObjectEntry("samples");
  json.beginArray();

  int i = 0;
  metric->scanSamples(
      fnord::util::DateTime::epoch(),
      fnord::util::DateTime::now(),
      [&json, &i] (Sample* sample) -> bool {
        if (i++ > 0) { json.addComma(); }
        json.beginObject();

        json.addObjectEntry("time");
        json.addLiteral<uint64_t>(static_cast<uint64_t>(sample->time()));
        json.addComma();

        json.addObjectEntry("value");
        json.addLiteral<double>(sample->value());
        json.addComma();

        json.addObjectEntry("labels");
        json.beginObject();
        auto labels = sample->labels();
        for (int n = 0; n < labels.size(); n++) {
          if (n > 0) {
            json.addComma();
          }

          json.addObjectEntry(labels[n].first);
          json.addString(labels[n].second);
        }
        json.endObject();

        json.endObject();
        return true;
      });

  json.endArray();
  json.endObject();

  response->setStatus(xzero::HttpStatus::Ok);
  response->addHeader("Content-Type", "application/json; charset=utf-8");
  response->setContentLength(output_stream->size());
  response->output()->write(
      std::move(output_stream->take()),
      std::bind(&xzero::HttpResponse::completed, response));
}

void HTTPAPI::executeQuery(
    xzero::HttpRequest* request,
    xzero::HttpResponse* response,
    util::URI* uri) {
  std::shared_ptr<util::InputStream> input_stream = createInputStream(request);

  std::shared_ptr<BufferOutputStream> output_stream(new BufferOutputStream());

  // FIXPAUL move to thread/worker pool
  query::QueryService query_service;
  std::unique_ptr<query::TableRepository> table_repo(
      new MetricTableRepository(metric_repo_));

  try {
    query_service.executeQuery(
        input_stream,
        query::QueryService::FORMAT_JSON,
        output_stream,
        std::move(table_repo));

  } catch (util::RuntimeException e) {
    output_stream->clear();

    util::JSONOutputStream json(std::move(output_stream));
    json.beginObject();
    json.addObjectEntry("status");
    json.addString("error");
    json.addComma();
    json.addObjectEntry("error");
    json.addString(e.getMessage());
    json.endObject();
  }

  response->setStatus(xzero::HttpStatus::Ok);
  response->addHeader("Content-Type", "application/json; charset=utf-8");
  response->setContentLength(output_stream->size());
  response->output()->write(
      std::move(output_stream->take()),
      std::bind(&xzero::HttpResponse::completed, response));
}

void HTTPAPI::renderMetricJSON(
    Metric* metric,
    util::JSONOutputStream* json) const {
  json->beginObject();

  json->addObjectEntry("key");
  json->addString(metric->key());
  json->addComma();

  json->addObjectEntry("total_bytes");
  json->addLiteral<size_t>(metric->totalBytes());
  json->addComma();

  json->addObjectEntry("last_insert");
  json->addLiteral<uint64_t>(metric->lastInsertTime());
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

}
}
