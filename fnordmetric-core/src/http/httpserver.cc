/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/environment.h>
#include <fnordmetric/http/httpserver.h>
#include <fnordmetric/http/httprequest.h>
#include <fnordmetric/http/httpresponse.h>
#include <fnordmetric/util/runtimeexception.h>
#include <xzero/TimeSpan.h>
#include <xzero/http/HttpRequest.h>
#include <xzero/http/HttpResponse.h>
#include <xzero/http/HttpService.h>
#include <xzero/executor/ThreadedExecutor.h>
#include <xzero/net/IPAddress.h>
#include <xzero/net/InetConnector.h>
#include <xzero/support/libev/LibevScheduler.h>
#include <xzero/support/libev/LibevSelector.h>
#include <xzero/support/libev/LibevClock.h>
#include <ev++.h>

namespace fnord {
namespace http {

class X0Adapter : public xzero::HttpService::Handler {
public:

  X0Adapter(
      std::vector<std::unique_ptr<HTTPHandler>>* handlers,
      xzero::support::LibevScheduler* x0_scheduler,
      fnord::thread::TaskScheduler* request_scheduler) :
      handlers_(handlers),
      x0_scheduler_(x0_scheduler),
      request_scheduler_(request_scheduler) {}

  bool handleRequest(
      xzero::HttpRequest* request,
      xzero::HttpResponse* response) override {

    auto async_handler = std::bind(
        &X0Adapter::handleRequestAsync,
        this,
        request,
        response);

    request_scheduler_->run(thread::Task::create(async_handler));

    return true;
  }

  void handleRequestAsync(
      xzero::HttpRequest* request,
      xzero::HttpResponse* response) {
    auto res = new fnord::http::HTTPResponse();
    auto req = new fnord::http::HTTPRequest(
        request->unparsedMethod(),
        request->unparsedUri());

    for (const auto& handler : *handlers_) {
      if (handler->handleHTTPRequest(req, res)) {
        break;
      }
    }

    x0_scheduler_->execute([request, response, req, res] () {
      response->setStatus((xzero::HttpStatus) res->statusCode());

      for (const auto hdr : res->getHeaders()) {
        response->addHeader(hdr.first, hdr.second);
      }

      const auto& body = res->getBody();
      response->setContentLength(body.size());
      response->output()->write(
          body,
          std::bind(&xzero::HttpResponse::completed, response));

      delete res;
      delete req;
    });
  }

protected:
  std::vector<std::unique_ptr<HTTPHandler>>* handlers_;
  xzero::support::LibevScheduler* x0_scheduler_;
  fnord::thread::TaskScheduler* request_scheduler_;
};

void HTTPServer::addHandler(std::unique_ptr<HTTPHandler> handler) {
  handlers_.emplace_back(std::move(handler));
}

HTTPServer::HTTPServer(
    TaskScheduler* request_scheduler) :
    request_scheduler_(request_scheduler) {}

void HTTPServer::listen(int port) {
  xzero::IPAddress bind("0.0.0.0");
  xzero::TimeSpan idle = xzero::TimeSpan::fromSeconds(30);
  xzero::HttpService http;
  ::ev::loop_ref loop = ::ev::default_loop(0);
  xzero::support::LibevScheduler scheduler(loop);
  xzero::support::LibevSelector selector(loop);
  xzero::support::LibevClock clock(loop);

  auto inet = http.configureInet(&scheduler, &scheduler, &selector, &clock,
                                 idle, bind, port);
  inet->setBlocking(false);

  X0Adapter adapter(&handlers_, &scheduler, request_scheduler_);
  http.addHandler(&adapter);
  http.start();
  selector.select();
}

}
}
