/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fnord/base/exception.h>
#include <fnord/net/http/httpparser.h>
#include <fnord/net/http/httprequest.h>
#include <fnord/net/http/httpresponse.h>
#include <fnord/io/inputstream.h>
#include <fnord/test/unittest.h>

using namespace fnord::http;
using fnord::io::StringInputStream;
using fnord::io::StringOutputStream;

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
    response.addCookie("fnord", "bar", fnord::DateTime(1418571527495314));
    EXPECT_EQ(
        response.getHeader("Set-Cookie"),
        "fnord=bar; Expires=Sun, 14-Dec-2014 15:38:47 UTC");
  }

  {
    HTTPResponse response;
    response.addCookie(
        "fnord",
        "bar",
        fnord::DateTime::epoch(),
        "/blah");
    EXPECT_EQ(
        response.getHeader("Set-Cookie"), "fnord=bar; Path=/blah");
  }

  {
    HTTPResponse response;
    response.addCookie(
        "fnord",
        "bar",
        fnord::DateTime::epoch(),
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
        fnord::DateTime::epoch(),
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
        fnord::DateTime::epoch(),
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
