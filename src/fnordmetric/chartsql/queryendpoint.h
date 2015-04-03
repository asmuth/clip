/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_METRICDB_HTTPINTERFACE_H
#define _FNORDMETRIC_METRICDB_HTTPINTERFACE_H
#include <memory>
#include <fnord-base/uri.h>
#include <fnord-http/httpservice.h>
#include <fnord-http/httprequest.h>
#include <fnord-http/httpresponse.h>
#include <fnord-json/jsonoutputstream.h>

using namespace fnord;
namespace fnordmetric {
class IMetric;
class IMetricRepository;

class QueryEndpoint : public http::HTTPService {
public:

  QueryEndpoint(fnord::metric_service::IMetricRepository* metric_repo);

  void handleHTTPRequest(
      http::HTTPRequest* request,
      http::HTTPResponse* response) override;

protected:

  fnord::metric_service::IMetricRepository* metric_repo_;
};

}
#endif
