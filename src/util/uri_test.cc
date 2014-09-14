/**
 * This file is part of the "FnordStream" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <stdio.h>
#include "uri.h"
#include "runtimeexception.h"
#include "unittest.h"

using namespace fnordmetric::util;

UNIT_TEST(URITest);

TEST_CASE(URITest, TestSchemeAndAuthority, [] () {
  URI uri("fnord://myhost");

  EXPECT_EQ(uri.scheme(), "fnord");
  EXPECT_EQ(uri.host(), "myhost");
  EXPECT_EQ(uri.port(), 0);
});

TEST_CASE(URITest, TestSchemeAndAuthorityWithPort, [] () {
  URI uri("fnord://myhost:2345");

  EXPECT_EQ(uri.scheme(), "fnord");
  EXPECT_EQ(uri.host(), "myhost");
  EXPECT_EQ(uri.port(), 2345);
});

TEST_CASE(URITest, TestSchemeAndAuthorityWithUserInfo, [] () {
  URI uri("fnord://blah@myhost");

  EXPECT_EQ(uri.scheme(), "fnord");
  EXPECT_EQ(uri.userinfo(), "blah");
  EXPECT_EQ(uri.host(), "myhost");
  EXPECT_EQ(uri.port(), 0);
});

TEST_CASE(URITest, TestSchemeAndAuthorityWithUserInfoWithPort, [] () {
  URI uri("fnord://blah@myhost:2345");

  EXPECT_EQ(uri.scheme(), "fnord");
  EXPECT_EQ(uri.userinfo(), "blah");
  EXPECT_EQ(uri.host(), "myhost");
  EXPECT_EQ(uri.port(), 2345);
});

TEST_CASE(URITest, TestSchemeAndAuthorityWithUserInfoSub, [] () {
  URI uri("fnord://blah:asd@myhost");

  EXPECT_EQ(uri.scheme(), "fnord");
  EXPECT_EQ(uri.userinfo(), "blah:asd");
  EXPECT_EQ(uri.host(), "myhost");
  EXPECT_EQ(uri.port(), 0);
});

TEST_CASE(URITest, TestSchemeAndAuthorityWithUserInfoSubWithPort, [] () {
  URI uri("fnord://blah:asd@myhost:2345");

  EXPECT_EQ(uri.scheme(), "fnord");
  EXPECT_EQ(uri.userinfo(), "blah:asd");
  EXPECT_EQ(uri.host(), "myhost");
  EXPECT_EQ(uri.port(), 2345);
});

TEST_CASE(URITest, TestSchemeAndPath, [] () {
  URI uri("fnord:my/path");

  EXPECT_EQ(uri.scheme(), "fnord");
  EXPECT_EQ(uri.path(), "my/path");
});

TEST_CASE(URITest, TestSchemeAndPathWithLeadingSlash, [] () {
  URI uri("fnord:/my/path");

  EXPECT_EQ(uri.scheme(), "fnord");
  EXPECT_EQ(uri.path(), "/my/path");
});

TEST_CASE(URITest, TestSchemeAndPathWithQueryString, [] () {
  URI uri("fnord:/my/path?myquerystring");

  EXPECT_EQ(uri.scheme(), "fnord");
  EXPECT_EQ(uri.path(), "/my/path");
  EXPECT_EQ(uri.query(), "myquerystring");
});

TEST_CASE(URITest, TestSchemeAndPathWithQueryStringAndFragment, [] () {
  URI uri("fnord:/my/path?myquerystring#myfragment");

  EXPECT_EQ(uri.scheme(), "fnord");
  EXPECT_EQ(uri.path(), "/my/path");
  EXPECT_EQ(uri.query(), "myquerystring");
  EXPECT_EQ(uri.fragment(), "myfragment");
});

TEST_CASE(URITest, TestSchemeAndPathWithFragment, [] () {
  URI uri("fnord:/my/path#myfragment");

  EXPECT_EQ(uri.scheme(), "fnord");
  EXPECT_EQ(uri.path(), "/my/path");
  EXPECT_EQ(uri.fragment(), "myfragment");
});
