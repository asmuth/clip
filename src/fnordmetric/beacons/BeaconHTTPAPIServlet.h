/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2015 Paul Asmuth
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_METRIC_BEACONHTTPAPISERVLET_H
#define _FNORD_METRIC_BEACONHTTPAPISERVLET_H
#include "fnord-chart/areachart.h"
#include "fnord-chart/canvas.h"
#include "fnord-chart/linechart.h"
#include "fnord-chart/svgtarget.h"
#include "fnord-http/httpservice.h"
#include "fnord-metricdb/metricservice.h"
#include "fnord-metricdb/timeseriesquery.h"
#include "fnord-json/json.h"
#include "BeaconService.h"

namespace fnordmetric {

class BeaconHTTPAPIServlet : public fnord::http::HTTPService {
public:

  BeaconHTTPAPIServlet(RefPtr<BeaconService> beacon_service);

  void handleHTTPRequest(
      fnord::http::HTTPRequest* req,
      fnord::http::HTTPResponse* res);

protected:

  void updateBeacon(
      http::HTTPRequest* request,
      http::HTTPResponse* response,
      URI* uri);

  void listBeacons(
      http::HTTPRequest* request,
      http::HTTPResponse* response,
      URI* uri);

  RefPtr<BeaconService> beacon_service_;
};

}
#endif
