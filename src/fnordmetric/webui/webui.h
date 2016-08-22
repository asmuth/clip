/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <memory>
#include <fnordmetric/transport/http/httpservice.h>

namespace fnordmetric {

class WebUI : public http::HTTPService {
public:

  WebUI(const String& dynamic_asset_path = "");

  void handleHTTPRequest(
      http::HTTPRequest* request,
      http::HTTPResponse* response) override;

private:

  std::string getAppHTML() const;

  std::string getAssetFile(const std::string& file) const;

  void sendAsset(
      http::HTTPResponse* response,
      const std::string& asset_path,
      const std::string& content_type) const;

  std::string dynamic_asset_path_;
};

}
