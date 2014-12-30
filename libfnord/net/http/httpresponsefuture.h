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
#include "fnord/net/http/httpresponse.h"
#include "fnord/net/http/httpresponsehandler.h"
#include "fnord/net/http/httpclientconnection.h"
#include "fnord/thread/wakeup.h"
#include <memory>

namespace fnord {
namespace http {

class HTTPResponseFuture {
public:
  HTTPResponseFuture();
  ~HTTPResponseFuture();

  HTTPResponseFuture(const HTTPResponseFuture& other) = delete;
  HTTPResponseFuture& operator=(const HTTPResponseFuture& other) = delete;

  const HTTPResponse& get() const;
  void wait();
  thread::Wakeup* onReady();

  HTTPResponseHandler* responseHandler();
  void storeConnection(std::unique_ptr<HTTPClientConnection>&& conn);

protected:
  HTTPResponse res_;
  DefaultHTTPResponseHandler res_handler_;
  fnord::thread::Wakeup wakeup_;
  std::unique_ptr<HTTPClientConnection> conn_;
};


}
}
#endif
