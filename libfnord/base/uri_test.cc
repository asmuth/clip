/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <stdio.h>
#include "fnord/base/exception.h"
#include "fnord/base/uri.h"
#include "fnord/test/unittest.h"

using namespace fnord;

UNIT_TEST(URITest);

TEST_CASE(URITest, TestSchemeAndAuthority, [] () {
  URI uri("fnord://myhost");

  EXPECT_EQ(uri.scheme(), "fnord");
  EXPECT_EQ(uri.host(), "myhost");
  EXPECT_EQ(uri.port(), 0);

  EXPECT_EQ(uri.toString(), "fnord://myhost");
});

TEST_CASE(URITest, TestSchemeAndAuthorityWithPort, [] () {
  URI uri("fnord://myhost:2345");

  EXPECT_EQ(uri.scheme(), "fnord");
  EXPECT_EQ(uri.host(), "myhost");
  EXPECT_EQ(uri.port(), 2345);

  EXPECT_EQ(uri.toString(), "fnord://myhost:2345");
});

TEST_CASE(URITest, TestSchemeAndAuthorityWithUserInfo, [] () {
  URI uri("fnord://blah@myhost");

  EXPECT_EQ(uri.scheme(), "fnord");
  EXPECT_EQ(uri.userinfo(), "blah");
  EXPECT_EQ(uri.host(), "myhost");
  EXPECT_EQ(uri.port(), 0);

  EXPECT_EQ(uri.toString(), "fnord://blah@myhost");
});

TEST_CASE(URITest, TestSchemeAndAuthorityWithUserInfoWithPort, [] () {
  URI uri("fnord://blah@myhost:2345");

  EXPECT_EQ(uri.scheme(), "fnord");
  EXPECT_EQ(uri.userinfo(), "blah");
  EXPECT_EQ(uri.host(), "myhost");
  EXPECT_EQ(uri.port(), 2345);

  EXPECT_EQ(uri.toString(), "fnord://blah@myhost:2345");
});

TEST_CASE(URITest, TestSchemeAndAuthorityWithUserInfoSub, [] () {
  URI uri("fnord://blah:asd@myhost");

  EXPECT_EQ(uri.scheme(), "fnord");
  EXPECT_EQ(uri.userinfo(), "blah:asd");
  EXPECT_EQ(uri.host(), "myhost");
  EXPECT_EQ(uri.port(), 0);

  EXPECT_EQ(uri.toString(), "fnord://blah:asd@myhost");
});

TEST_CASE(URITest, TestSchemeAndAuthorityWithUserInfoSubWithPort, [] () {
  URI uri("fnord://blah:asd@myhost:2345");

  EXPECT_EQ(uri.scheme(), "fnord");
  EXPECT_EQ(uri.userinfo(), "blah:asd");
  EXPECT_EQ(uri.host(), "myhost");
  EXPECT_EQ(uri.port(), 2345);

  EXPECT_EQ(uri.toString(), "fnord://blah:asd@myhost:2345");
});

TEST_CASE(URITest, TestSchemeAndPath, [] () {
  URI uri("fnord:my/path");

  EXPECT_EQ(uri.scheme(), "fnord");
  EXPECT_EQ(uri.path(), "my/path");

  EXPECT_EQ(uri.toString(), "fnord:my/path");
});

TEST_CASE(URITest, TestSchemeAndPathAndQuery, [] () {
  URI uri("fnord:my/path?asdasd");

  EXPECT_EQ(uri.scheme(), "fnord");
  EXPECT_EQ(uri.path(), "my/path");
  EXPECT_EQ(uri.query(), "asdasd");

  EXPECT_EQ(uri.toString(), "fnord:my/path?asdasd");
});

TEST_CASE(URITest, TestSchemeAndPathWithLeadingSlash, [] () {
  URI uri("fnord:/my/path");

  EXPECT_EQ(uri.scheme(), "fnord");
  EXPECT_EQ(uri.path(), "/my/path");

  EXPECT_EQ(uri.toString(), "fnord:/my/path");
});

TEST_CASE(URITest, TestSchemeAndPathWithQueryString, [] () {
  URI uri("fnord:/my/path?myquerystring");

  EXPECT_EQ(uri.scheme(), "fnord");
  EXPECT_EQ(uri.path(), "/my/path");
  EXPECT_EQ(uri.query(), "myquerystring");

  EXPECT_EQ(uri.toString(), "fnord:/my/path?myquerystring");
});

TEST_CASE(URITest, TestSchemeAndPathWithQueryStringAndFragment, [] () {
  URI uri("fnord:/my/path?myquerystring#myfragment");

  EXPECT_EQ(uri.scheme(), "fnord");
  EXPECT_EQ(uri.path(), "/my/path");
  EXPECT_EQ(uri.query(), "myquerystring");
  EXPECT_EQ(uri.fragment(), "myfragment");

  EXPECT_EQ(uri.toString(), "fnord:/my/path?myquerystring#myfragment");
});

TEST_CASE(URITest, TestSchemeAndPathWithFragment, [] () {
  URI uri("fnord:/my/path#myfragment");

  EXPECT_EQ(uri.scheme(), "fnord");
  EXPECT_EQ(uri.path(), "/my/path");
  EXPECT_EQ(uri.fragment(), "myfragment");

  EXPECT_EQ(uri.toString(), "fnord:/my/path#myfragment");
});

TEST_CASE(URITest, TestParseQueryParamsSingle, [] () {
  URI uri("fnord:path?fuu=bar");

  auto params = uri.queryParams();
  EXPECT_EQ(params.size(), 1);
  EXPECT_EQ(params[0].first, "fuu");
  EXPECT_EQ(params[0].second, "bar");

  EXPECT_EQ(uri.toString(), "fnord:path?fuu=bar");
});

TEST_CASE(URITest, TestParseQueryParams, [] () {
  URI uri("fnord:path?fuu=bar&blah=123123");

  auto params = uri.queryParams();
  EXPECT_EQ(params.size(), 2);
  EXPECT_EQ(params[0].first, "fuu");
  EXPECT_EQ(params[0].second, "bar");
  EXPECT_EQ(params[1].first, "blah");
  EXPECT_EQ(params[1].second, "123123");

  EXPECT_EQ(uri.toString(), "fnord:path?fuu=bar&blah=123123");
});
