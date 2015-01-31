/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_HTTPROUTER_H
#define _FNORDMETRIC_HTTPROUTER_H

#include "fnord/net/http/httphandler.h"
#include "fnord/net/http/httprequest.h"
#include "fnord/net/http/httpresponse.h"
#include "fnord-base/thread/taskscheduler.h"

namespace fnord {
namespace http {
class HTTPService;

class HTTPRouter : public HTTPHandlerFactory {
public:
  typedef std::function<bool (HTTPRequest*)> PredicateFnType;

  typedef std::function<
      std::unique_ptr<HTTPHandler> (
          HTTPServerConnection*,
          HTTPRequest*)> FactoryFnType;

  template <typename... HandlerArgs>
  void addRouteByPrefixMatch(
      const std::string& prefix,
      HandlerArgs... handler);

  void addRoute(
      PredicateFnType predicate,
      HTTPService* service);

  void addRoute(
      PredicateFnType predicate,
      HTTPService* service,
      TaskScheduler* scheduler);

  void addRoute(
      PredicateFnType predicate,
      HTTPHandlerFactory* factory);

  std::unique_ptr<HTTPHandler> getHandler(
      HTTPServerConnection* conn,
      HTTPRequest* req) override;

protected:

  class NoSuchRouteHandler : public HTTPHandler {
  public:
    NoSuchRouteHandler(HTTPServerConnection* conn, HTTPRequest* req);
    void handleHTTPRequest() override;
  protected:
    HTTPServerConnection* conn_;
    HTTPRequest* req_;
    HTTPResponse res_;
  };


  std::vector<std::pair<PredicateFnType, FactoryFnType>> routes_;
};

}
}
#include "httprouter_impl.h"
#endif
