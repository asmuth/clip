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
    RefPtr<BeaconService> beacon_service) :
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

    if (StringUtil::endsWith(uri.path(), "/list")) {
      return listBeacons(req, res, &uri);
    }

    if (StringUtil::endsWith(uri.path(), "/history")) {
      return fetchBeaconHistory(req, res, &uri);
    }

    res->setStatus(fnord::http::kStatusNotFound);
    res->addBody("not found");
  } catch (const Exception& e) {
    res->setStatus(http::kStatusInternalServerError);
    res->addBody(StringUtil::format("error: $0", e.getMessage()));
  }
}

void BeaconHTTPAPIServlet::listBeacons(
    http::HTTPRequest* request,
    http::HTTPResponse* response,
    URI* uri) {
  auto params = uri->queryParams();

  response->setStatus(http::kStatusOK);
  response->addHeader("Content-Type", "application/json; charset=utf-8");
  json::JSONOutputStream jsons(response->getBodyOutputStream());

  jsons.beginObject();
  jsons.addObjectEntry("beacons");
  jsons.beginArray();

  String prefix;
  fnord::URI::getParam(params, "prefix", &prefix);

  int i = 0;
  beacon_service_->listBeacons(prefix, BeaconStatus::ANY, [&jsons, &i] (
      const String& key,
      BeaconStatus status,
      const DateTime& last_seen,
      const Option<DateTime>& expect_next,
      const Option<String>& status_text,
      const Option<URI>& status_url) -> bool {
    if (++i > 1) {
      jsons.addComma();
    }

    jsons.beginObject();

    jsons.addObjectEntry("key");
    jsons.addString(key);

    jsons.addComma();
    jsons.addObjectEntry("last_seen");
    jsons.addString(StringUtil::toString(last_seen.unixMicros()));

    if (!expect_next.isEmpty()) {
      jsons.addComma();
      jsons.addObjectEntry("expect_next");
      jsons.addString(StringUtil::toString(expect_next.get().unixMicros()));
    }

    jsons.addComma();
    jsons.addObjectEntry("status");
    jsons.addString(status == BeaconStatus::HEALTHY ? "healthy" : "unhealthy");

    if (!status_text.isEmpty()) {
      jsons.addComma();
      jsons.addObjectEntry("status_text");
      jsons.addString(status_text.get());
    }

    if (!status_url.isEmpty()) {
      jsons.addComma();
      jsons.addObjectEntry("status_uri");
      jsons.addString(status_url.get().toString());
    }

    jsons.endObject();
    return true;
  });

  jsons.endArray();
  jsons.endObject();
}

void BeaconHTTPAPIServlet::fetchBeaconHistory(
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

  response->setStatus(http::kStatusOK);
  response->addHeader("Content-Type", "application/json; charset=utf-8");
  json::JSONOutputStream jsons(response->getBodyOutputStream());

  jsons.beginObject();
  jsons.addObjectEntry("key");
  jsons.addString(beacon_key);
  jsons.addComma();
  jsons.addObjectEntry("history");
  jsons.beginArray();

  int i = 0;
  auto begin = DateTime::epoch();
  auto end = DateTime{};
  beacon_service_->fetchBeaconHistory(beacon_key, begin, end, [&jsons, &i] (
      const DateTime& time,
      BeaconStatus status,
      const Option<String>& status_text,
      const Option<URI>& status_url) -> bool {
    if (++i > 1) {
      jsons.addComma();
    }

    jsons.beginObject();

    jsons.addObjectEntry("time");
    jsons.addString(StringUtil::toString(time.unixMicros()));

    jsons.addComma();
    jsons.addObjectEntry("status");
    jsons.addString(status == BeaconStatus::HEALTHY ? "healthy" : "unhealthy");

    if (!status_text.isEmpty()) {
      jsons.addComma();
      jsons.addObjectEntry("status_text");
      jsons.addString(status_text.get());
    }

    if (!status_url.isEmpty()) {
      jsons.addComma();
      jsons.addObjectEntry("status_uri");
      jsons.addString(status_url.get().toString());
    }

    jsons.endObject();
    return true;
  });

  jsons.endArray();
  jsons.endObject();
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
