/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_WEBINTERFACE_H
#define _FNORDMETRIC_WEBINTERFACE_H
#include <memory>
#include <xzero/http/HttpService.h>

namespace fnordmetric {
namespace metricdb {

class AdminUI : public xzero::HttpService::Handler {
public:

  static xzero::HttpService::Handler* get();

  bool handleRequest(
      xzero::HttpRequest* request,
      xzero::HttpResponse* response) override;
};

}
}
#endif
