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
#include <fnordmetric/util/jsonoutputstream.h>
#include <fnordmetric/util/uri.h>
#include <xzero/http/HttpService.h>

namespace fnordmetric {
namespace metricdb {
class IMetric;
class IMetricRepository;

class HTTPAPI : public xzero::HttpService::Handler {
public:

  HTTPAPI(IMetricRepository* metric_repo);

  bool handleRequest(
      xzero::HttpRequest* request,
      xzero::HttpResponse* response) override;

protected:

  void renderMetricList(
      xzero::HttpRequest* request,
      xzero::HttpResponse* response,
      util::URI* uri);

  void renderMetricSampleScan(
      xzero::HttpRequest* request,
      xzero::HttpResponse* response,
      util::URI* uri);

  void insertSample(
      xzero::HttpRequest* request,
      xzero::HttpResponse* response,
      util::URI* uri);

  void executeQuery(
      xzero::HttpRequest* request,
      xzero::HttpResponse* response,
      util::URI* uri);

  void renderMetricJSON(
      IMetric* metric,
      util::JSONOutputStream* json) const;

  IMetricRepository* metric_repo_;
};

}
}
#endif
