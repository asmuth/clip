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
#include <metrictools/listen.h>
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

class HTTPServer: public Task {
public:

  HTTPServer(const ConfigList* config, Backend* storage_backend);

  ReturnCode listenAndRun(const std::string& addr, int port);

  void start() override;
  void shutdown() override;

  void setAssetPath(const std::string& path);

protected:

  void handleRequest(
      http::HTTPRequest* request,
      http::HTTPResponse* response);

  void handleRequest_PLOT(
      http::HTTPRequest* request,
      http::HTTPResponse* response);

  std::string getPreludeHTML() const;
  std::string getAppHTML() const;
  std::string getAssetFile(const std::string& file) const;

  void sendAsset(
      http::HTTPResponse* response,
      const std::string& asset_path,
      const std::string& content_type) const;

  const ConfigList* config_;
  Backend* storage_backend_;
  libtransport::http::HTTPServer http_server_;
  std::string dynamic_asset_path_;
  std::thread thread_;
};

struct ListenHTTPTaskConfig : public ListenerConfig {
  ListenHTTPTaskConfig();
  std::string bind;
  uint16_t port;
};

ReturnCode startHTTPListener(
    Backend* storage_backend,
    const ConfigList* config,
    const ListenerConfig* task_config,
    std::unique_ptr<Task>* task);

} // namespace fnordmetric

