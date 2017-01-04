/**
 * This file is part of the "libstx" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V.
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include "libtransport/json/json.h"
#include "libtransport/json/json_reader.h"
#include "libtransport/json/json_writer.h"
#include "libtransport/json/json_object.h"
#include "unittest.h"

using namespace libtransport::json;

UNIT_TEST(JSONTest);

TEST_CASE(JSONTest, TestJSONWriter, [] () {
  std::string json_str;
  JSONWriter json(&json_str);
  json.beginObject();
  json.addString("123");
  json.addString("fnord");
  json.addString("blah");
  json.beginArray();
  json.addTrue();
  json.addFalse();
  json.addNull();
  json.endArray();
  json.endObject();
  EXPECT(json_str == R"({"123":"fnord","blah":[true,false,null]})");
});

TEST_CASE(JSONTest, TestJSONReader, [] () {
  std::string json1 = "{ 123: \"fnord\", \"blah\": [ true, false, null, 3.7e-5 ] }";
  JSONReader json1_stream(&json1);

  kTokenType token_type;
  std::string token_str;

  EXPECT_TRUE(json1_stream.readNextToken(&token_type, &token_str));
  EXPECT_EQ(token_type, JSON_OBJECT_BEGIN);

  EXPECT_TRUE(json1_stream.readNextToken(&token_type, &token_str));
  EXPECT_EQ(token_type, JSON_NUMBER);
  EXPECT_EQ(token_str, "123");

  EXPECT_TRUE(json1_stream.readNextToken(&token_type, &token_str));
  EXPECT_EQ(token_type, JSON_STRING);
  EXPECT_EQ(token_str, "fnord");

  EXPECT_TRUE(json1_stream.readNextToken(&token_type, &token_str));
  EXPECT_EQ(token_type, JSON_STRING);
  EXPECT_EQ(token_str, "blah");

  EXPECT_TRUE(json1_stream.readNextToken(&token_type, &token_str));
  EXPECT_EQ(token_type, JSON_ARRAY_BEGIN);

  EXPECT_TRUE(json1_stream.readNextToken(&token_type, &token_str));
  EXPECT_EQ(token_type, JSON_TRUE);

  EXPECT_TRUE(json1_stream.readNextToken(&token_type, &token_str));
  EXPECT_EQ(token_type, JSON_FALSE);

  EXPECT_TRUE(json1_stream.readNextToken(&token_type, &token_str));
  EXPECT_EQ(token_type, JSON_NULL);

  EXPECT_TRUE(json1_stream.readNextToken(&token_type, &token_str));
  EXPECT_EQ(token_type, JSON_NUMBER);
  EXPECT_EQ(token_str, "3.7e-5");

  EXPECT_TRUE(json1_stream.readNextToken(&token_type, &token_str));
  EXPECT_EQ(token_type, JSON_ARRAY_END);

  EXPECT_TRUE(json1_stream.readNextToken(&token_type, &token_str));
  EXPECT_EQ(token_type, JSON_OBJECT_END);
});

TEST_CASE(JSONTest, TestJSONReaderUnescaping, [] () {
  std::string json1 = "{ 123: \"fno\\\"rd\" }";
  JSONReader json1_stream(&json1);

  kTokenType token_type;
  std::string token_str;

  EXPECT_TRUE(json1_stream.readNextToken(&token_type, &token_str));
  EXPECT_EQ(token_type, JSON_OBJECT_BEGIN);

  EXPECT_TRUE(json1_stream.readNextToken(&token_type, &token_str));
  EXPECT_EQ(token_type, JSON_NUMBER);
  EXPECT_EQ(token_str, "123");

  EXPECT_TRUE(json1_stream.readNextToken(&token_type, &token_str));
  EXPECT_EQ(token_type, JSON_STRING);
  EXPECT_EQ(token_str, "fno\"rd");

  EXPECT_TRUE(json1_stream.readNextToken(&token_type, &token_str));
  EXPECT_EQ(token_type, JSON_OBJECT_END);
});

TEST_CASE(JSONTest, TestJSONReaderToObject, [] () {
  std::string json_str = "{ 123: \"fnord\", \"blah\": [ true, false, null, 3.7e-5 ] }";

  JSONStorage json_elem;
  auto rc = readJSON(&json_elem, &json_str);
  EXPECT_TRUE(rc);

  EXPECT_TRUE(json_elem.hasRootObject());
  auto obj = json_elem.getRootAsObject();

  EXPECT_TRUE(obj->has("123"));
  EXPECT(obj->getString("123") == "fnord");

  EXPECT_TRUE(obj->has("blah"));
  EXPECT(obj->getArray("blah")->size() == 4);
  EXPECT(obj->getArray("blah")->getBoolean(0) == true);
  EXPECT(obj->getArray("blah")->getBoolean(1) == false);
  EXPECT(obj->getArray("blah")->get(2)->isNull());
  EXPECT(obj->getArray("blah")->getDouble(3) == 0.000037);
});

TEST_CASE(JSONTest, TestMultiLevelEscaping, [] () {
  std::string json_str = R"({"str":"fub \\\"blah\\\" bar"})";

  JSONStorage json_obj;
  auto rc = readJSON(&json_obj, &json_str);
  EXPECT_TRUE(rc);

  EXPECT_TRUE(json_obj.hasRootObject());
  EXPECT_TRUE(json_obj.getRootAsObject()->has("str"));
  EXPECT_TRUE(json_obj.getRootAsObject()->getString("str") ==  "fub \\\"blah\\\" bar");
});

TEST_CASE(JSONTest, TestJSONObjecStorage, [] () {
  JSONObjectStorage json_obj;
  json_obj->setString("test", "fnord");
  auto arr = json_obj->setArray("test2");
  arr->appendString("xxx");
  arr->appendUInt64(1234);

  std::string json_str;
  auto rc = writeJSON(json_obj.getRoot(), &json_str);
  EXPECT_TRUE(rc);

  EXPECT(json_str == R"({"test":"fnord","test2":["xxx",1234]})");
});

TEST_CASE(JSONTest, TestJSONReencoding1, [] () {
  std::string orig_str = "{\"123\":\"fnord\",\"blah\":[true,false,null,3.7e-5]}";

  JSONStorage json_obj;
  auto rc = readJSON(&json_obj, &orig_str);
  EXPECT_TRUE(rc);

  std::string new_string;
  rc = writeJSON(json_obj.getRoot(), &new_string);
  EXPECT_TRUE(rc);

  EXPECT(orig_str == new_string);
});

TEST_CASE(JSONTest, TestJSONReencoding2, [] () {
  std::string orig_str = R"({"customer":"xxx","docid":{"set_id":"p","item_id":"123"},"attributes":{"title~en":"12- blubb silver version","description~en":"925 blah blahb blah -- \"12\" represents 12 different chinese zodiac sign animals 鼠 Rat is symbol of Charm  those who born in 1948、1960、1972、1984 、1996 and 2008 are belonged to Rat  product description: it is a pendant with 2 faces 1 face is shinny well-polished silver surface the other face is matt look finishing with a raw feeling therefore, this big pendant can be wear in both sides to match your apparel!  the pendant is linked by a 100cm/ 40 inch long silver plated chain.  pendant size: 8.5cm x 7cm, about 15 gr.  the necklace would be received with our gift packing especially for rat @\"12\" series!!  ******************************************************  you are welcome to visit my web-site","category2":"999"}})";

  JSONStorage json_obj;
  auto rc = readJSON(&json_obj, &orig_str);
  EXPECT_TRUE(rc);

  std::string new_string;
  rc = writeJSON(json_obj.getRoot(), &new_string);
  EXPECT_TRUE(rc);

  EXPECT(orig_str == new_string);
});

TEST_CASE(JSONTest, TestNested, [] () {
  JSONArrayStorage json_arr;
  json_arr->appendObject();
  json_arr->appendObject();
  json_arr->appendObject();

  std::string json_str;
  auto rc = writeJSON(json_arr.getRoot(), &json_str);
  EXPECT_TRUE(rc);

  EXPECT(json_str == "[{},{},{}]");
});

