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
#include "fnord/base/io/inputstream.h"
#include "fnord/json/flatjsonreader.h"
#include "fnord/json/jsondocument.h"
#include "fnord/json/jsonutil.h"
#include "fnord/json/jsoninputstream.h"
#include "fnord/json/jsonpointer.h"
#include "fnord/test/unittest.h"

UNIT_TEST(JSONTest);

using fnord::StringInputStream;
using fnord::json::FlatJSONReader;
using fnord::json::JSONDocument;
using fnord::json::JSONInputStream;
using fnord::json::JSONPointer;
using fnord::json::JSONUtil;

TEST_CASE(JSONTest, TestJSONInputStream, [] () {
  auto json1 = "{ 123: \"fnord\", \"blah\": [ true, false, null, 3.7e-5 ] }";
  JSONInputStream json1_stream(StringInputStream::fromString(json1));

  fnord::json::kTokenType token_type;
  std::string token_str;

  EXPECT_TRUE(json1_stream.readNextToken(&token_type, &token_str));
  EXPECT_EQ(token_type, fnord::json::JSON_OBJECT_BEGIN);

  EXPECT_TRUE(json1_stream.readNextToken(&token_type, &token_str));
  EXPECT_EQ(token_type, fnord::json::JSON_NUMBER);
  EXPECT_EQ(token_str, "123");

  EXPECT_TRUE(json1_stream.readNextToken(&token_type, &token_str));
  EXPECT_EQ(token_type, fnord::json::JSON_STRING);
  EXPECT_EQ(token_str, "fnord");

  EXPECT_TRUE(json1_stream.readNextToken(&token_type, &token_str));
  EXPECT_EQ(token_type, fnord::json::JSON_STRING);
  EXPECT_EQ(token_str, "blah");

  EXPECT_TRUE(json1_stream.readNextToken(&token_type, &token_str));
  EXPECT_EQ(token_type, fnord::json::JSON_ARRAY_BEGIN);

  EXPECT_TRUE(json1_stream.readNextToken(&token_type, &token_str));
  EXPECT_EQ(token_type, fnord::json::JSON_TRUE);

  EXPECT_TRUE(json1_stream.readNextToken(&token_type, &token_str));
  EXPECT_EQ(token_type, fnord::json::JSON_FALSE);

  EXPECT_TRUE(json1_stream.readNextToken(&token_type, &token_str));
  EXPECT_EQ(token_type, fnord::json::JSON_NULL);

  EXPECT_TRUE(json1_stream.readNextToken(&token_type, &token_str));
  EXPECT_EQ(token_type, fnord::json::JSON_NUMBER);
  EXPECT_EQ(token_str, "3.7e-5");

  EXPECT_TRUE(json1_stream.readNextToken(&token_type, &token_str));
  EXPECT_EQ(token_type, fnord::json::JSON_ARRAY_END);

  EXPECT_TRUE(json1_stream.readNextToken(&token_type, &token_str));
  EXPECT_EQ(token_type, fnord::json::JSON_OBJECT_END);
});

TEST_CASE(JSONTest, TestJSONInputStreamEscaping, [] () {
  auto json1 = "{ 123: \"fno\\\"rd\" }";
  JSONInputStream json1_stream(StringInputStream::fromString(json1));

  fnord::json::kTokenType token_type;
  std::string token_str;

  EXPECT_TRUE(json1_stream.readNextToken(&token_type, &token_str));
  EXPECT_EQ(token_type, fnord::json::JSON_OBJECT_BEGIN);

  EXPECT_TRUE(json1_stream.readNextToken(&token_type, &token_str));
  EXPECT_EQ(token_type, fnord::json::JSON_NUMBER);
  EXPECT_EQ(token_str, "123");

  EXPECT_TRUE(json1_stream.readNextToken(&token_type, &token_str));
  EXPECT_EQ(token_type, fnord::json::JSON_STRING);
  EXPECT_EQ(token_str, "fno\"rd");

  EXPECT_TRUE(json1_stream.readNextToken(&token_type, &token_str));
  EXPECT_EQ(token_type, fnord::json::JSON_OBJECT_END);
});

TEST_CASE(JSONTest, TestJSONDocumentGet, [] () {
  auto json1 = "{ 123: \"fnord\", \"blah\": [ true, false, null, 3.7e-5 ] }";
  JSONDocument json1_doc(json1);

  EXPECT_EQ(json1_doc.get("/123"), "fnord");
  EXPECT_EQ(json1_doc.get("/blah/0"), "true");
  EXPECT_EQ(json1_doc.get("/blah/1"), "false");
  EXPECT_EQ(json1_doc.get("/blah/2"), "null");
  EXPECT_EQ(json1_doc.get("/blah/3"), "3.7e-5");
});

TEST_CASE(JSONTest, TestJSONDocumentForEach, [] () {
  auto json1 = "{ 123: \"fnord\", \"blah\": [ true, false, null, 3.7e-5 ] }";
  JSONDocument json1_doc(json1);

  int n = 0;
  json1_doc.forEach(JSONPointer("/"), [&n] (const JSONPointer& path) -> bool {
    if (n == 0) { EXPECT_EQ(path.toString(), "/123"); }
    if (n == 1) { EXPECT_EQ(path.toString(), "/blah"); }
    EXPECT_TRUE(n++ < 2);
    return true;
  });

  n = 0;
  json1_doc.forEach(JSONPointer("/blah"), [&n] (
      const JSONPointer& path) -> bool {
    if (n == 0) { EXPECT_EQ(path.toString(), "/blah/0"); }
    if (n == 1) { EXPECT_EQ(path.toString(), "/blah/1"); }
    if (n == 2) { EXPECT_EQ(path.toString(), "/blah/2"); }
    if (n == 3) { EXPECT_EQ(path.toString(), "/blah/3"); }
    EXPECT_TRUE(n++ < 4);
    return true;
  });

  n = 0;
  json1_doc.forEach(JSONPointer("/blah/"), [&n] (
      const JSONPointer& path) -> bool {
    if (n == 0) { EXPECT_EQ(path.toString(), "/blah/0"); }
    if (n == 1) { EXPECT_EQ(path.toString(), "/blah/1"); }
    if (n == 2) { EXPECT_EQ(path.toString(), "/blah/2"); }
    if (n == 3) { EXPECT_EQ(path.toString(), "/blah/3"); }
    EXPECT_TRUE(n++ < 4);
    return true;
  });
});

TEST_CASE(JSONTest, TestJSONPointerEscaping, [] () {
  std::string str1 = "blah~fnord/bar/12~35";
  JSONPointer::escape(&str1);
  EXPECT_EQ(str1, "blah~0fnord~1bar~112~035");
});

typedef std::vector<std::pair<std::string, std::string>> VectorOfStringPairs;

TEST_CASE(JSONTest, TestFlatJSONReader, [] () {
  std::string str1 = "blah~fnord/bar/12~35";
  JSONPointer::escape(&str1);
  EXPECT_EQ(str1, "blah~0fnord~1bar~112~035");

  auto json1 = "{ 123: \"fnord\", \"blah\": [ true, false, null, 3.7e-5 ] }";
  FlatJSONReader json1_reader(
      JSONInputStream(StringInputStream::fromString(json1)));

  VectorOfStringPairs expected;
  expected.emplace_back(std::make_pair("/123", "fnord"));
  expected.emplace_back(std::make_pair("/blah/0", "true"));
  expected.emplace_back(std::make_pair("/blah/1", "false"));
  expected.emplace_back(std::make_pair("/blah/2", "null"));
  expected.emplace_back(std::make_pair("/blah/3", "3.7e-5"));

  int n = 0;
  json1_reader.read([&] (
      const JSONPointer& path,
      const std::string& value) -> bool {
    EXPECT_TRUE(n < expected.size());
    EXPECT_EQ(expected[n].first, path.toString());
    EXPECT_EQ(expected[n].second, value);
    n++;
    return true;
  });
});

TEST_CASE(JSONTest, TestFromJSON, [] () {
  EXPECT_EQ(fnord::json::fromJSON<std::string>("\"fnord\""), "fnord");

  auto json1 = fnord::json::parseJSON(
      "{ 123: \"fnord\", \"blubb\": \"xxx\", \"blah\": [ true, false, null, " \
      "3.7e-5 ] }");

  auto iter = JSONUtil::objectLookup(json1.begin(), json1.end(), "blubb");
  EXPECT_TRUE(iter != json1.end());
  EXPECT_EQ(fnord::json::fromJSON<std::string>(iter, json1.end()), "xxx");

  iter = JSONUtil::objectLookup(json1.begin(), json1.end(), "blah");
  EXPECT_TRUE(iter != json1.end());
  EXPECT_EQ(iter->type, fnord::json::JSON_ARRAY_BEGIN);
  EXPECT_EQ(JSONUtil::arrayLength(iter, json1.end()), 4);

  auto aiter = JSONUtil::arrayLookup(iter, json1.end(), 0);
  EXPECT_EQ(fnord::json::fromJSON<std::string>(aiter, json1.end()), "true");
  aiter = JSONUtil::arrayLookup(iter, json1.end(), 1);
  EXPECT_EQ(fnord::json::fromJSON<std::string>(aiter, json1.end()), "false");
  aiter = JSONUtil::arrayLookup(iter, json1.end(), 2);
  EXPECT_EQ(fnord::json::fromJSON<std::string>(aiter, json1.end()), "null");
  aiter = JSONUtil::arrayLookup(iter, json1.end(), 3);
  EXPECT_EQ(fnord::json::fromJSON<std::string>(aiter, json1.end()), "3.7e-5");
});

TEST_CASE(JSONTest, TestToJSON, [] () {
  fnord::json::JSONObject j1;
  fnord::json::toJSON(std::string("blah"), &j1);

  EXPECT_EQ(j1.size(), 1);
  EXPECT_EQ(j1[0].type, fnord::json::JSON_STRING);
  EXPECT_EQ(j1[0].data, "blah");
});

struct TestMessage {
  std::string a;
  int b;

  template <typename T>
  static void reflect(T* meta) {
    meta->prop(&TestMessage::a, 1, "a", false);
    meta->prop(&TestMessage::b, 2, "b", false);
  }
};

TEST_CASE(JSONTest, TestToFromJSON, [] () {
  TestMessage m1;
  m1.a = "stringdata";
  m1.b = 23;

  auto j1 = fnord::json::parseJSON(fnord::json::toJSONString(m1));
  auto m2 = fnord::json::fromJSON<TestMessage>(j1);

  EXPECT_EQ(m1.a, m2.a);
  EXPECT_EQ(m1.b, m2.b);
});

/*
TEST_CASE(JSONTest, TestParseJSON, [] () {
  auto json = fnord::json::parseJSON(R"(
    {
       "jsonrpc":"2.0",
       "id":"0",
       "result":[
          {
             "a":"0",
             "b":"c",
             "data":"xxx|bbb|v=3&1,2,4~5,"
          }
       ]
    }
  )");

  EXPECT_EQ(json.size(), 17);
});
*/
