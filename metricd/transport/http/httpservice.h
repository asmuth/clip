/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _libstx_HTTPSERVICE_H
#define _libstx_HTTPSERVICE_H
#include <metricd/transport/http/httphandler.h>
#include <metricd/transport/http/httprequest.h>
#include <metricd/transport/http/httpresponse.h>
#include <metricd/transport/http/HTTPRequestStream.h>
#include <metricd/transport/http/HTTPResponseStream.h>
#include "metricd/transport/http/taskscheduler.h"

namespace fnordmetric {
namespace http {

class StreamingHTTPService {
public:

  virtual ~StreamingHTTPService() {}

  virtual void handleHTTPRequest(
      RefPtr<HTTPRequestStream> req,
      RefPtr<HTTPResponseStream> res) = 0;

  virtual bool isStreaming() {
    return true;
  }

};

class HTTPService : public StreamingHTTPService {
public:

  virtual ~HTTPService() {}

  virtual void handleHTTPRequest(
      HTTPRequest* req,
      HTTPResponse* res) = 0;

  void handleHTTPRequest(
      RefPtr<HTTPRequestStream> req,
      RefPtr<HTTPResponseStream> res) override;

  bool isStreaming() override {
    return false;
  }

};

class HTTPServiceHandler : public HTTPHandler {
public:
  HTTPServiceHandler(
      StreamingHTTPService* service,
      TaskScheduler* scheduler,
      HTTPServerConnection* conn,
      HTTPRequest* req);

  void handleHTTPRequest() override;

protected:
  void dispatchRequest();

  StreamingHTTPService* service_;
  TaskScheduler* scheduler_;
  HTTPServerConnection* conn_;
  HTTPRequest* req_;
};

}
}
#endif
