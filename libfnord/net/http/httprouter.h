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

namespace fnord {
namespace http {
class HTTPService;

class HTTPRouter : public HTTPHandlerFactory {
public:

  template <typename HandlerType>
  void addRouteByPrefixMatch(
      const std::string& prefix,
      HandlerType handler);

  void addRoute(
      std::function<bool (HTTPRequest*)> predicate,
      HTTPService* service);

  void addRoute(
      std::function<bool (HTTPRequest*)> predicate,
      HTTPHandlerFactory* factory);

  std::unique_ptr<HTTPHandler> getHandler(
      HTTPConnection* conn,
      HTTPRequest* req) override;

protected:

  class NoSuchRouteHandler : public HTTPHandler {
  public:
    NoSuchRouteHandler(HTTPConnection* conn, HTTPRequest* req);
    void handleHTTPRequest() override;
  protected:
    HTTPConnection* conn_;
    HTTPRequest* req_;
    HTTPResponse res_;
  };

  typedef std::function<bool (HTTPRequest*)> PredicateFnType;
  typedef std::function<
      std::unique_ptr<HTTPHandler> (
          HTTPConnection*,
          HTTPRequest*)> FactoryFnType;

  std::vector<std::pair<PredicateFnType, FactoryFnType>> routes_;
};

}
}
#include "httprouter_impl.h"
#endif
