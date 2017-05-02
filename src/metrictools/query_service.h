/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *   Copyright (c) 2017 Paul Asmuth <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <memory>
#include <libtransport/http/v1/http_server.h>
#include <libtransport/http/http_request.h>
#include <libtransport/http/http_response.h>
#include <libtransport/uri/uri.h>
#include <metrictools/backends/backend.h>

namespace fnordmetric {
using namespace libtransport;

class QueryService {
public:

  QueryService(Backend* backend);

  ReturnCode listenAndRun(const std::string& addr, int port);

  void setAssetPath(const std::string& path);

protected:

  void handleRequest(
      http::HTTPRequest* request,
      http::HTTPResponse* response);

  void handleRequest_QUERY(
      http::HTTPRequest* request,
      http::HTTPResponse* response);

  std::string getPreludeHTML() const;
  std::string getAppHTML() const;
  std::string getAssetFile(const std::string& file) const;

  void sendAsset(
      http::HTTPResponse* response,
      const std::string& asset_path,
      const std::string& content_type) const;

  Backend* backend_;
  libtransport::http::HTTPServer http_server_;
  std::string dynamic_asset_path_;
};

} // namespace fnordmetric

