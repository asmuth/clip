/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V. <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <metrictools/ingest.h>
#include <metrictools/webui/webui.h>
#include <libtransport/http/v1/http_server.h>
#include <libtransport/http/http_request.h>
#include <libtransport/http/http_response.h>
#include <libtransport/uri/uri.h>
#include <libtransport/json/json.h>
#include <libtransport/json/json_reader.h>
#include <libtransport/json/json_writer.h>

namespace fnordmetric {
class Backend;

namespace json = libtransport::json;
namespace http = libtransport::http;

struct HTTPPushIngestionTaskConfig : public IngestionTaskConfig {
  HTTPPushIngestionTaskConfig();
  std::string bind;
  uint16_t port;
};

class HTTPPushIngestionTask : public IngestionTask {
public:

  static ReturnCode start(
      Backend* storage_backend,
      const IngestionTaskConfig* config,
      std::unique_ptr<IngestionTask>* task);

  HTTPPushIngestionTask(Backend* storage_backend);

  ReturnCode listen(const std::string& addr, int port);

  virtual void start() override;
  virtual void shutdown() override;

protected:

  void handleRequest(
      http::HTTPRequest* request,
      http::HTTPResponse* response);

  Backend* storage_backend_;
  libtransport::http::HTTPServer http_server_;
};

} // namespace fnordmetric

