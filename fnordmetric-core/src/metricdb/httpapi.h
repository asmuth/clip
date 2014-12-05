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
#include <fnord/base/uri.h>
#include <fnord/net/http/httphandler.h>
#include <fnord/net/http/httprequest.h>
#include <fnord/net/http/httpresponse.h>
#include <fnord/format/json/jsonoutputstream.h>

using namespace fnord;
namespace fnordmetric {
namespace metricdb {
class IMetric;
class IMetricRepository;

class HTTPAPI : public http::HTTPHandler {
public:

  HTTPAPI(IMetricRepository* metric_repo);

  bool handleHTTPRequest(
      http::HTTPRequest* request,
      http::HTTPResponse* response) override;

protected:

  void renderMetricList(
      http::HTTPRequest* request,
      http::HTTPResponse* response,
      fnord::URI* uri);

  void renderMetricSampleScan(
      http::HTTPRequest* request,
      http::HTTPResponse* response,
      fnord::URI* uri);

  void insertSample(
      http::HTTPRequest* request,
      http::HTTPResponse* response,
      fnord::URI* uri);

  void executeQuery(
      http::HTTPRequest* request,
      http::HTTPResponse* response,
      fnord::URI* uri);

  void renderMetricJSON(
      IMetric* metric,
      json::JSONOutputStream* json) const;

  IMetricRepository* metric_repo_;
};

}
}
#endif
