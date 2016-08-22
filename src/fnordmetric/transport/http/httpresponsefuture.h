/**
 * Copyright (c) 2016 zScale Technology GmbH <legal@zscale.io>
 * Authors:
 *   - Paul Asmuth <paul@zscale.io>
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License ("the license") as
 * published by the Free Software Foundation, either version 3 of the License,
 * or any later version.
 *
 * In accordance with Section 7(e) of the license, the licensing of the Program
 * under the license does not imply a trademark license. Therefore any rights,
 * title and interest in our trademarks remain entirely with us.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the license for more details.
 *
 * You can be released from the requirements of the license by purchasing a
 * commercial license. Buying such a license is mandatory as soon as you develop
 * commercial activities involving this program without disclosing the source
 * code of your own applications
 */
#ifndef _libstx_HTTPRESPONSEFUTURE_H
#define _libstx_HTTPRESPONSEFUTURE_H
#include "fnordmetric/util/status.h"
#include "fnordmetric/transport/http/httpresponse.h"
#include "fnordmetric/transport/http/httpresponsehandler.h"
#include "fnordmetric/transport/http/httpclientconnection.h"
#include "fnordmetric/util/thread/future.h"
#include "fnordmetric/util/thread/wakeup.h"
#include <memory>

namespace fnordmetric {
namespace http {

class HTTPResponseFuture : public HTTPResponseHandler {
public:
  HTTPResponseFuture(Promise<HTTPResponse> promise);
  virtual ~HTTPResponseFuture();

  HTTPResponseFuture(const HTTPResponseFuture& other) = delete;
  HTTPResponseFuture& operator=(const HTTPResponseFuture& other) = delete;

  void storeConnection(std::unique_ptr<HTTPClientConnection>&& conn);

  virtual void onError(const std::exception& e) override;
  virtual void onVersion(const std::string& version) override;
  virtual void onStatusCode(int status_code) override;
  virtual void onStatusName(const std::string& status) override;
  virtual void onHeader(const std::string& key, const std::string& value) override;
  virtual void onHeadersComplete() override;
  virtual void onBodyChunk(const char* data, size_t size) override;
  virtual void onResponseComplete() override;

protected:
  HTTPResponse res_;
  Promise<HTTPResponse> promise_;
  std::unique_ptr<HTTPClientConnection> conn_;
};

class StreamingResponseHandler : public HTTPResponseFuture {
public:
  typedef Function<void (const char* data, size_t size)> CallbackFn;

  typedef
      Function<HTTPResponseFuture* (const Promise<HTTPResponse>)>
      FactoryFn;

  static FactoryFn getFactory(CallbackFn on_event);

  StreamingResponseHandler(
      Promise<HTTPResponse> promise,
      CallbackFn callback);

  void onBodyChunk(const char* data, size_t size) override;

protected:
  CallbackFn callback_;
};

using StreamingResponseFuture = StreamingResponseHandler;

}
}
#endif
