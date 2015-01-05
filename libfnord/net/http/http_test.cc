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
#include <fnord/net/http/httpinputstream.h>
#include <fnord/net/http/httpoutputstream.h>
#include <fnord/net/http/httprequest.h>
#include <fnord/net/http/httpresponse.h>
#include <fnord/io/inputstream.h>
#include <fnord/test/unittest.h>

using namespace fnord::http;
using fnord::http::HTTPInputStream;
using fnord::http::HTTPOutputStream;
using fnord::io::StringInputStream;
using fnord::io::StringOutputStream;

UNIT_TEST(HTTPTest);

TEST_CASE(HTTPTest, ParseHTTP1dot0Request, [] () {
  auto req = "GET / HTTP/1.0\r\n" \
             "\r\n";

  StringInputStream is(req);
  HTTPInputStream http_is(&is);
  HTTPRequest request;
  request.readFromInputStream(&http_is);

  EXPECT_EQ(request.getMethod(), "GET");
  EXPECT_EQ(request.getUrl(), "/");
  EXPECT_EQ(request.getVersion(), "HTTP/1.0");
  EXPECT_EQ(request.keepalive(), false);
});

TEST_CASE(HTTPTest, ParseHTTP1dot0KeepaliveRequest, [] () {
  auto req = "GET / HTTP/1.0\r\n" \
             "Connection: keep-alive\r\n" \
             "\r\n";

  StringInputStream is(req);
  HTTPInputStream http_is(&is);
  HTTPRequest request;
  request.readFromInputStream(&http_is);

  EXPECT_EQ(request.getMethod(), "GET");
  EXPECT_EQ(request.getUrl(), "/");
  EXPECT_EQ(request.getVersion(), "HTTP/1.0");
  EXPECT_EQ(request.keepalive(), true);
});

TEST_CASE(HTTPTest, ParseHTTP1dot1Request, [] () {
  auto req = "GET / HTTP/1.1\r\n" \
             "\r\n";

  StringInputStream is(req);
  HTTPInputStream http_is(&is);
  HTTPRequest request;
  request.readFromInputStream(&http_is);

  EXPECT_EQ(request.getMethod(), "GET");
  EXPECT_EQ(request.getUrl(), "/");
  EXPECT_EQ(request.getVersion(), "HTTP/1.1");
  EXPECT_EQ(request.keepalive(), true);
});

TEST_CASE(HTTPTest, PopulateHTTPResponseFromHTTP1dot1Request, [] () {
  auto req = "GET / HTTP/1.1\r\n" \
             "\r\n";

  StringInputStream is(req);
  HTTPInputStream http_is(&is);
  HTTPRequest request;
  request.readFromInputStream(&http_is);
  EXPECT_TRUE(request.keepalive() == true);

  HTTPResponse response;
  response.populateFromRequest(request);

  EXPECT_EQ(response.getVersion(), "HTTP/1.1");
});

TEST_CASE(HTTPTest, PopulateHTTPResponseFromHTTP1dot0Request, [] () {
  auto req = "GET / HTTP/1.0\r\n" \
             "\r\n";

  StringInputStream is(req);
  HTTPInputStream http_is(&is);
  HTTPRequest request;
  request.readFromInputStream(&http_is);

  HTTPResponse response;
  response.populateFromRequest(request);

  EXPECT_EQ(response.getVersion(), "HTTP/1.0");
  EXPECT_EQ(response.getHeader("Connection"), "close");
});

TEST_CASE(HTTPTest, PopulateHTTPResponseFromHTTP1dot0KeepaliveRequest, [] () {
  auto req = "GET / HTTP/1.0\r\n" \
             "Connection: keep-alive\r\n" \
             "\r\n";

  StringInputStream is(req);
  HTTPInputStream http_is(&is);
  HTTPRequest request;
  request.readFromInputStream(&http_is);

  HTTPResponse response;
  response.populateFromRequest(request);

  EXPECT_EQ(response.getVersion(), "HTTP/1.0");
  EXPECT_EQ(response.getHeader("Connection"), "keep-alive");
});
