/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2015 Paul Asmuth
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnord-base/option.h>
#include <fnord-base/duration.h>
#include "BeaconHTTPAPIServlet.h"

namespace fnordmetric {

BeaconHTTPAPIServlet::BeaconHTTPAPIServlet(
    BeaconService* beacon_service) :
    beacon_service_(beacon_service) {}

void BeaconHTTPAPIServlet::handleHTTPRequest(
    fnord::http::HTTPRequest* req,
    fnord::http::HTTPResponse* res) {
  URI uri(req->uri());

  res->addHeader("Access-Control-Allow-Origin", "*");

  try {
    if (StringUtil::endsWith(uri.path(), "/update")) {
      return updateBeacon(req, res, &uri);
    }

    res->setStatus(fnord::http::kStatusNotFound);
    res->addBody("not found");
  } catch (const Exception& e) {
    res->setStatus(http::kStatusInternalServerError);
    res->addBody(StringUtil::format("error: $0", e.getMessage()));
  }
}

void BeaconHTTPAPIServlet::updateBeacon(
    http::HTTPRequest* request,
    http::HTTPResponse* response,
    URI* uri) {
  auto params = uri->queryParams();

  String beacon_key;
  if (!fnord::URI::getParam(params, "key", &beacon_key)) {
    response->addBody("error: missing ?key=... parameter");
    response->setStatus(http::kStatusBadRequest);
    return;
  }

  String status_str;
  if (!fnord::URI::getParam(params, "status", &status_str)) {
    response->addBody("error: missing ?status=... parameter");
    response->setStatus(http::kStatusBadRequest);
    return;
  }

  BeaconStatus status;
  if (status_str == "healthy") {
    status = BeaconStatus::HEALTHY;
  } else if (status_str == "unhealthy") {
    status = BeaconStatus::UNHEALTHY;
  } else {
    response->addBody("error: invalid ?status=... parameter, "\
        "must be 'healthy' or 'unhealthy'");
    response->setStatus(http::kStatusBadRequest);
    return;
  }

  String status_text_str;
  Option<String> status_text;
  if (fnord::URI::getParam(params, "status_text", &status_text_str)) {
    status_text = Some(status_text_str);
  }

  Option<URI> status_uri;
  String status_uri_str;
  if (fnord::URI::getParam(params, "status_uri", &status_uri_str)) {
    status_uri = Some(URI(status_uri_str));
  }

  Option<Duration> expect_next;
  String expect_next_str;
  if (fnord::URI::getParam(params, "expect_next", &expect_next_str)) {
    expect_next = Some(Duration(std::stoul(expect_next_str)));
  }

  beacon_service_->updateBeacon(
      beacon_key,
      status,
      status_text,
      status_uri,
      expect_next);

  response->setStatus(http::kStatusCreated);
  response->addBody("ok");
}

}
