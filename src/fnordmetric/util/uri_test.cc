/**
 * This file is part of the "FnordMetric" project
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
#include "eventql/util/exception.h"
#include "eventql/util/uri.h"
#include "eventql/util/test/unittest.h"

#include "eventql/eventql.h"

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

TEST_CASE(URITest, TestWeirdUrls, [] () {
  URI uri1(
      "/t.gif?c=f9765c4564e077c0cb~4ae4a27f81fa&e=q&qstr:de=x" \
          "xx&is=p~40938238~1,p~70579299~2");

  EXPECT_EQ(uri1.path(), "/t.gif");
  EXPECT_EQ(
      uri1.query(),
      "c=f9765c4564e077c0cb~4ae4a27f81fa&e=q&qstr:de=x" \
          "xx&is=p~40938238~1,p~70579299~2");
});

TEST_CASE(URITest, TestWeirdUrlDecode, [] () {
  EXPECT_EQ(URI::urlDecode("8%+elasthan"), "8% elasthan");
});

TEST_CASE(URITest, TestUrlDecode, [] () {
  EXPECT_EQ(URI::urlDecode("filz%20tasche"), "filz tasche");
  EXPECT_EQ(URI::urlDecode("fu%2Fbar%3F12%3Dasd"), "fu/bar?12=asd");
  EXPECT_EQ(URI::urlDecode("%C3%B6"), "ö");
  EXPECT_EQ(URI::urlDecode("fu%252Fbar%253F12%253Dasd"), "fu%2Fbar%3F12%3Dasd");
  EXPECT_EQ(
      URI::urlDecode("%40%22~.%2F%2F%3Bq6%5C%27%7B%7D42%25%7B"),
      "@\"~.//;q6\\'{}42%{");
});

TEST_CASE(URITest, TestUrlEncode, [] () {
  EXPECT_EQ(URI::urlEncode("filz tasche"), "filz%20tasche");
  EXPECT_EQ(URI::urlEncode("ö"), "%C3%B6");
  EXPECT_EQ(URI::urlEncode("fu/bar?12=asd"), "fu%2Fbar%3F12%3Dasd");
  EXPECT_EQ(URI::urlEncode("fu%2Fbar%3F12%3Dasd"),"fu%252Fbar%253F12%253Dasd");
  EXPECT_EQ(
      URI::urlEncode("@\"~.//;q6\\'{}42%{"),
      "%40%22~.%2F%2F%3Bq6%5C%27%7B%7D42%25%7B");
});


