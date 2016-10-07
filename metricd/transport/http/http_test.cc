/**
 * Copyright (c) 2016 zScale Technology GmbH <legal@zscale.io>
 * Authors:
 *   - Paul Asmuth <paul@zscale.io>
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License ("the license") as
 * published by the Free Software Foundation, either version 3 of the License,
 * or any later version.
 *
 * In accordance with Section 7(e) of the license, the licensing of the Program
 * under the license does not imply a trademark license. Therefore any rights,
 * title and interest in our trademarks remain entirely with us.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the license for more details.
 *
 * You can be released from the requirements of the license by purchasing a
 * commercial license. Buying such a license is mandatory as soon as you develop
 * commercial activities involving this program without disclosing the source
 * code of your own applications
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fnordmetric/util/exception.h>
#include <fnordmetric/transport/http/httpclient.h>
#include <fnordmetric/transport/http/httpconnectionpool.h>
#include <fnordmetric/transport/http/httpparser.h>
#include <fnordmetric/transport/http/httprequest.h>
#include <fnordmetric/transport/http/httpresponse.h>
#include <fnordmetric/transport/http/httpresponsehandler.h>
#include <fnordmetric/transport/http/httpclientconnection.h>
#include <fnordmetric/util/io/inputstream.h>
#include <fnordmetric/util/test/unittest.h>
#include <fnordmetric/util/thread/eventloop.h>
#include <fnordmetric/util/thread/threadpool.h>

#include "fnordmetric/eventql.h"
using namespace http;

UNIT_TEST(HTTPTest);

TEST_CASE(HTTPTest, ParseHTTP1dot0Request, [] () {
  auto request = HTTPRequest::parse(
      "GET / HTTP/1.0\r\n" \
      "\r\n");

  EXPECT_EQ(request.method(), HTTPMessage::M_GET);
  EXPECT_EQ(request.uri(), "/");
  EXPECT_EQ(request.version(), "HTTP/1.0");
  EXPECT_EQ(request.keepalive(), false);
});

TEST_CASE(HTTPTest, ParseHTTP1dot0KeepaliveRequest, [] () {
  auto request = HTTPRequest::parse(
      "GET / HTTP/1.0\r\n" \
       "Connection: keep-alive\r\n" \
       "\r\n");

  EXPECT_EQ(request.method(), HTTPMessage::M_GET);
  EXPECT_EQ(request.uri(), "/");
  EXPECT_EQ(request.version(), "HTTP/1.0");
  EXPECT_EQ(request.keepalive(), true);
});

TEST_CASE(HTTPTest, ParseHTTP1dot1Request, [] () {
  auto request = HTTPRequest::parse(
      "GET / HTTP/1.1\r\n" \
       "\r\n");

  EXPECT_EQ(request.method(), HTTPMessage::M_GET);
  EXPECT_EQ(request.uri(), "/");
  EXPECT_EQ(request.version(), "HTTP/1.1");
  EXPECT_EQ(request.keepalive(), true);
});

TEST_CASE(HTTPTest, PopulateHTTPResponseFromHTTP1dot1Request, [] () {
  auto request = HTTPRequest::parse(
      "GET / HTTP/1.1\r\n" \
      "\r\n");

  EXPECT_TRUE(request.keepalive() == true);

  HTTPResponse response;
  response.populateFromRequest(request);
  EXPECT_EQ(response.version(), "HTTP/1.1");
});

TEST_CASE(HTTPTest, ParseHTTP1dot0Response, [] () {
  auto response = HTTPResponse::parse(
      "HTTP/1.0 302 Found\r\n" \
      "fnord: bar\r\n" \
      "\r\n");

  EXPECT_EQ(response.version(), "HTTP/1.0");
  EXPECT_EQ(response.statusCode(), 302);
  EXPECT_EQ(response.statusName(), "Found");
  EXPECT_EQ(response.headers().size(), 1);
  EXPECT_EQ(response.getHeader("fnord"), "bar");
});

TEST_CASE(HTTPTest, ParseHTTP1dot0ResponseWithBodyAndContentLength, [] () {
  auto response = HTTPResponse::parse(
      "HTTP/1.0 200 OK\r\n" \
      "Content-Length: 5\r\n" \
      "\r\nfnord");

  EXPECT_EQ(response.version(), "HTTP/1.0");
  EXPECT_EQ(response.statusCode(), 200);
  EXPECT_EQ(response.statusName(), "OK");
  EXPECT_EQ(response.headers().size(), 1);
  EXPECT_EQ(response.body().size(), 5);
  EXPECT_EQ(response.body().toString(), "fnord");
});

TEST_CASE(HTTPTest, ParseHTTP1dot0ResponseWithBodyWithoutContentLength, [] () {
  auto response = HTTPResponse::parse(
      "HTTP/1.0 200 OK\r\n" \
      "\r\nblah");

  EXPECT_EQ(response.version(), "HTTP/1.0");
  EXPECT_EQ(response.statusCode(), 200);
  EXPECT_EQ(response.statusName(), "OK");
  EXPECT_EQ(response.headers().size(), 0);
  EXPECT_EQ(response.body().size(), 4);
  EXPECT_EQ(response.body().toString(), "blah");
});

TEST_CASE(HTTPTest, PopulateHTTPResponseFromHTTP1dot0Request, [] () {
  auto request = HTTPRequest::parse(
      "GET / HTTP/1.0\r\n" \
      "\r\n");

  HTTPResponse response;
  response.populateFromRequest(request);

  EXPECT_EQ(response.version(), "HTTP/1.0");
  EXPECT_EQ(response.getHeader("Connection"), "close");
});

TEST_CASE(HTTPTest, PopulateHTTPResponseFromHTTP1dot0KeepaliveRequest, [] () {
  auto request = HTTPRequest::parse(
      "GET / HTTP/1.0\r\n" \
      "Connection: keep-alive\r\n" \
      "\r\n");

  HTTPResponse response;
  response.populateFromRequest(request);

  EXPECT_EQ(response.version(), "HTTP/1.0");
  EXPECT_EQ(response.getHeader("Connection"), "keep-alive");
});

TEST_CASE(HTTPTest, TestAddCookie, [] () {
  {
    HTTPResponse response;
    response.addCookie("fnord", "bar");
    EXPECT_EQ(response.getHeader("Set-Cookie"), "fnord=bar");
  }

  {
    HTTPResponse response;
    response.addCookie("fnord", "bar", UnixTime(1418571527495314));
    EXPECT_EQ(
        response.getHeader("Set-Cookie"),
        "fnord=bar; Expires=Sun, 14-Dec-2014 15:38:47 UTC");
  }

  {
    HTTPResponse response;
    response.addCookie(
        "fnord",
        "bar",
        UnixTime::epoch(),
        "/blah");
    EXPECT_EQ(
        response.getHeader("Set-Cookie"), "fnord=bar; Path=/blah");
  }

  {
    HTTPResponse response;
    response.addCookie(
        "fnord",
        "bar",
        UnixTime::epoch(),
        "",
        ".fnrd.net");
    EXPECT_EQ(
        response.getHeader("Set-Cookie"), "fnord=bar; Domain=.fnrd.net");
  }

  {
    HTTPResponse response;
    response.addCookie(
        "fnord",
        "bar",
        UnixTime::epoch(),
        "",
        "",
        false,
        true);
    EXPECT_EQ(
        response.getHeader("Set-Cookie"), "fnord=bar; HttpOnly");
  }

  {
    HTTPResponse response;
    response.addCookie(
        "fnord",
        "bar",
        UnixTime::epoch(),
        "",
        "",
        true,
        false);
    EXPECT_EQ(
        response.getHeader("Set-Cookie"), "fnord=bar; Secure");
  }
});

TEST_CASE(HTTPTest, TestGetCookies, [] () {
  auto request = HTTPRequest::parse(
      "GET / HTTP/1.0\r\n" \
       "Cookie: __utma=1.1377480910.1406310643.1416763484.1416773800.23" \
       "; __utmz=1.1406310643.1.1.utmcsr=(direct)|utmccn=(direct)|utmcm" \
       "d=(none); _u=fnordbar\r\n" \
       "\r\n");

  auto cookies = request.cookies();
  EXPECT_EQ(cookies.size(), 3);
  EXPECT_EQ(cookies[0].first, "__utma");
  EXPECT_EQ(
      cookies[0].second,
      "1.1377480910.1406310643.1416763484.1416773800.23");
  EXPECT_EQ(cookies[1].first, "__utmz");
  EXPECT_EQ(
      cookies[1].second,
      "1.1406310643.1.1.utmcsr=(direct)|utmccn=(direct)|utmcmd=(none)");
  EXPECT_EQ(cookies[2].first, "_u");
  EXPECT_EQ(cookies[2].second, "fnordbar");
});

TEST_CASE(HTTPTest, TestInvalidCookies, [] () {
  auto request = HTTPRequest::parse(
      "GET / HTTP/1.0\r\n" \
      "Cookie: _u=fnord; _x; =blah; =\r\n" \
      "\r\n");

  auto cookies = request.cookies();
  EXPECT_TRUE(cookies.size() >= 1);
  EXPECT_EQ(cookies[0].first, "_u");
  EXPECT_EQ(cookies[0].second, "fnord");
});

//TEST_CASE(HTTPTest, TestHTTPConnectionPoolEnd2End, [] () {
//  thread::ThreadPool tp;
//  HTTPConnectionPool http_pool(&tp);
//
//  auto res = http_pool.executeRequest(
//      http::HTTPRequest::mkGet("http://www.google.com/"));
//  res.wait();
//
//  const auto& r = res.get();
//  EXPECT_EQ(r.statusCode(), 200);
//});

TEST_CASE(HTTPTest, TestHTTPClient, [] () {
  HTTPClient client(nullptr);

  auto req = HTTPRequest::mkGet("http://www.heise.de/");
  auto res = client.executeRequest(req);

  EXPECT_EQ(res.statusCode(), 200);
});
