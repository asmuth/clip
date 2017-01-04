/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "unittest.h"
#include "libtransport/http/http_request.h"
#include "libtransport/http/http_response.h"

using namespace libtransport::http;

UNIT_TEST(HTTPTest);

TEST_CASE(HTTPTest, ParseHTTP1dot0Request, [] () {
  HTTPRequest request;
  request.parse(
      "GET / HTTP/1.0\r\n" \
      "\r\n");

  EXPECT_EQ(request.method(), HTTPMessage::M_GET);
  EXPECT_EQ(request.uri(), "/");
  EXPECT_EQ(request.version(), "HTTP/1.0");
  EXPECT_EQ(request.keepalive(), false);
});

TEST_CASE(HTTPTest, ParseHTTP1dot0KeepaliveRequest, [] () {
  HTTPRequest request;
  request.parse(
      "GET / HTTP/1.0\r\n" \
       "Connection: keep-alive\r\n" \
       "\r\n");

  EXPECT_EQ(request.method(), HTTPMessage::M_GET);
  EXPECT_EQ(request.uri(), "/");
  EXPECT_EQ(request.version(), "HTTP/1.0");
  EXPECT_EQ(request.keepalive(), true);
});

TEST_CASE(HTTPTest, ParseHTTP1dot1Request, [] () {
  HTTPRequest request;
  request.parse(
      "GET / HTTP/1.1\r\n" \
       "\r\n");

  EXPECT_EQ(request.method(), HTTPMessage::M_GET);
  EXPECT_EQ(request.uri(), "/");
  EXPECT_EQ(request.version(), "HTTP/1.1");
  EXPECT_EQ(request.keepalive(), true);
});

TEST_CASE(HTTPTest, PopulateHTTPResponseFromHTTP1dot1Request, [] () {
  HTTPRequest request;
  request.parse(
      "GET / HTTP/1.1\r\n" \
      "\r\n");

  EXPECT_TRUE(request.keepalive() == true);

  HTTPResponse response;
  response.populateFromRequest(request);
  EXPECT_EQ(response.version(), "HTTP/1.1");
});

TEST_CASE(HTTPTest, ParseHTTP1dot0Response, [] () {
  HTTPResponse response;
  response.parse(
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
  HTTPResponse response;
  response.parse(
      "HTTP/1.0 200 OK\r\n" \
      "Content-Length: 5\r\n" \
      "\r\nfnord");

  EXPECT_EQ(response.version(), "HTTP/1.0");
  EXPECT_EQ(response.statusCode(), 200);
  EXPECT_EQ(response.statusName(), "OK");
  EXPECT_EQ(response.headers().size(), 1);
  EXPECT_EQ(response.body().size(), 5);
  EXPECT_EQ(response.body(), "fnord");
});

TEST_CASE(HTTPTest, ParseHTTP1dot0ResponseWithBodyWithoutContentLength, [] () {
  HTTPResponse response;
  response.parse(
      "HTTP/1.0 200 OK\r\n" \
      "\r\nblah");

  EXPECT_EQ(response.version(), "HTTP/1.0");
  EXPECT_EQ(response.statusCode(), 200);
  EXPECT_EQ(response.statusName(), "OK");
  EXPECT_EQ(response.headers().size(), 0);
  EXPECT_EQ(response.body().size(), 4);
  EXPECT_EQ(response.body(), "blah");
});

TEST_CASE(HTTPTest, PopulateHTTPResponseFromHTTP1dot0Request, [] () {
  HTTPRequest request;
  request.parse(
      "GET / HTTP/1.0\r\n" \
      "\r\n");

  HTTPResponse response;
  response.populateFromRequest(request);

  EXPECT_EQ(response.version(), "HTTP/1.0");
  EXPECT_EQ(response.getHeader("Connection"), "close");
});

TEST_CASE(HTTPTest, PopulateHTTPResponseFromHTTP1dot0KeepaliveRequest, [] () {
  HTTPRequest request;
  request.parse(
      "GET / HTTP/1.0\r\n" \
      "Connection: keep-alive\r\n" \
      "\r\n");

  HTTPResponse response;
  response.populateFromRequest(request);

  EXPECT_EQ(response.version(), "HTTP/1.0");
  EXPECT_EQ(response.getHeader("Connection"), "keep-alive");
});

//TEST_CASE(HTTPTest, TestAddCookie, [] () {
//  {
//    HTTPResponse response;
//    response.addCookie("fnord", "bar");
//    EXPECT_EQ(response.getHeader("Set-Cookie"), "fnord=bar");
//  }
//
//  {
//    HTTPResponse response;
//    response.addCookie("fnord", "bar", UnixTime(1418571527495314));
//    EXPECT_EQ(
//        response.getHeader("Set-Cookie"),
//        "fnord=bar; Expires=Sun, 14-Dec-2014 15:38:47 UTC");
//  }
//
//  {
//    HTTPResponse response;
//    response.addCookie(
//        "fnord",
//        "bar",
//        UnixTime::epoch(),
//        "/blah");
//    EXPECT_EQ(
//        response.getHeader("Set-Cookie"), "fnord=bar; Path=/blah");
//  }
//
//  {
//    HTTPResponse response;
//    response.addCookie(
//        "fnord",
//        "bar",
//        UnixTime::epoch(),
//        "",
//        ".fnrd.net");
//    EXPECT_EQ(
//        response.getHeader("Set-Cookie"), "fnord=bar; Domain=.fnrd.net");
//  }
//
//  {
//    HTTPResponse response;
//    response.addCookie(
//        "fnord",
//        "bar",
//        UnixTime::epoch(),
//        "",
//        "",
//        false,
//        true);
//    EXPECT_EQ(
//        response.getHeader("Set-Cookie"), "fnord=bar; HttpOnly");
//  }
//
//  {
//    HTTPResponse response;
//    response.addCookie(
//        "fnord",
//        "bar",
//        UnixTime::epoch(),
//        "",
//        "",
//        true,
//        false);
//    EXPECT_EQ(
//        response.getHeader("Set-Cookie"), "fnord=bar; Secure");
//  }
//});

//TEST_CASE(HTTPTest, TestGetCookies, [] () {
//  auto request = HTTPRequest::parse(
//      "GET / HTTP/1.0\r\n" \
//       "Cookie: __utma=1.1377480910.1406310643.1416763484.1416773800.23" \
//       "; __utmz=1.1406310643.1.1.utmcsr=(direct)|utmccn=(direct)|utmcm" \
//       "d=(none); _u=fnordbar\r\n" \
//       "\r\n");
//
//  auto cookies = request.cookies();
//  EXPECT_EQ(cookies.size(), 3);
//  EXPECT_EQ(cookies[0].first, "__utma");
//  EXPECT_EQ(
//      cookies[0].second,
//      "1.1377480910.1406310643.1416763484.1416773800.23");
//  EXPECT_EQ(cookies[1].first, "__utmz");
//  EXPECT_EQ(
//      cookies[1].second,
//      "1.1406310643.1.1.utmcsr=(direct)|utmccn=(direct)|utmcmd=(none)");
//  EXPECT_EQ(cookies[2].first, "_u");
//  EXPECT_EQ(cookies[2].second, "fnordbar");
//});
//
//TEST_CASE(HTTPTest, TestInvalidCookies, [] () {
//  auto request = HTTPRequest::parse(
//      "GET / HTTP/1.0\r\n" \
//      "Cookie: _u=fnord; _x; =blah; =\r\n" \
//      "\r\n");
//
//  auto cookies = request.cookies();
//  EXPECT_TRUE(cookies.size() >= 1);
//  EXPECT_EQ(cookies[0].first, "_u");
//  EXPECT_EQ(cookies[0].second, "fnord");
//});
//
