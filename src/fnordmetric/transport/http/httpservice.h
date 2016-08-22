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
#ifndef _libstx_HTTPSERVICE_H
#define _libstx_HTTPSERVICE_H
#include <fnordmetric/transport/http/httphandler.h>
#include <fnordmetric/transport/http/httprequest.h>
#include <fnordmetric/transport/http/httpresponse.h>
#include <fnordmetric/transport/http/HTTPRequestStream.h>
#include <fnordmetric/transport/http/HTTPResponseStream.h>
#include "fnordmetric/util/thread/taskscheduler.h"

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
