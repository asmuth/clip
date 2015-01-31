/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_HTTPRESPONSEFUTURE_H
#define _FNORDMETRIC_HTTPRESPONSEFUTURE_H
#include "fnord-base/status.h"
#include "fnord/net/http/httpresponse.h"
#include "fnord/net/http/httpresponsehandler.h"
#include "fnord/net/http/httpclientconnection.h"
#include "fnord-base/thread/future.h"
#include "fnord-base/thread/wakeup.h"
#include <memory>

namespace fnord {
namespace http {

class HTTPResponseFuture : public HTTPResponseHandler {
public:
  HTTPResponseFuture(Promise<HTTPResponse> promise);
  ~HTTPResponseFuture();

  HTTPResponseFuture(const HTTPResponseFuture& other) = delete;
  HTTPResponseFuture& operator=(const HTTPResponseFuture& other) = delete;

  void storeConnection(std::unique_ptr<HTTPClientConnection>&& conn);

  void onError(const std::exception& e) override;
  void onVersion(const std::string& version) override;
  void onStatusCode(int status_code) override;
  void onStatusName(const std::string& status) override;
  void onHeader(const std::string& key, const std::string& value) override;
  void onHeadersComplete() override;
  void onBodyChunk(const char* data, size_t size) override;
  void onResponseComplete() override;

protected:
  HTTPResponse res_;
  Promise<HTTPResponse> promise_;
  std::unique_ptr<HTTPClientConnection> conn_;
};


}
}
#endif
